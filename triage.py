"""Triaje de issues con Jev.

Lo ejecuta el workflow `.github/workflows/bugtracking.yml` cada vez que se abre una
issue: le hace a Jev (vía OpenRouter) cinco preguntas sobre la issue y aplica
las etiquetas en las que Jev está seguro. Si no lo está, añade `needs-triage`.
"""

import json
import os

import requests

# Versión fijada: el alias "latest" podría cambiar las respuestas sin avisar.
MODEL = "typesafe/jev-1.13"

MIN_CONFIDENCE = 0.6  # choice/score: por debajo no se etiqueta y se pide revisión
MIN_YES = 0.8  # noul: probabilidad de "sí" a partir de la cual se etiqueta
SEVERITY = ["wishlist", "low", "medium", "high", "critical"]  # el índice es el valor del score

# cOLORES
MORADO = "6f42c1"
VERDE = "0e8a16"  
AMARILLO = "fbca04"  
NARANJA = "d93f0b"  
ROJO = "b60205"  

# DICCIONARIO PARA ASIGNAR A CADA SEVERIDAD SU COLOR
SEVERITY_COLORS = {
    "wishlist": MORADO,
    "low": VERDE,
    "medium": AMARILLO,
    "high": NARANJA,
    "critical": ROJO,
}

QUESTIONS = {
    "tipo": {
        "type": "choice",
        "instructions": "¿Qué tipo de issue es la descrita en `issue`?",
        "criteria": {
            "bug": "Algo que existe falla o se comporta distinto a lo esperado",
            "feature": "Se pide una funcionalidad nueva o una mejora",
            "question": "Una duda de uso; no se reporta ningún fallo",
            "docs": "Error, carencia o mejora en la documentación",
        },
    },
    "severidad": {
        "type": "score",
        "instructions": (
            "¿Qué gravedad tiene el defecto o solicitud descrita en `issue`? "
            "Para funcionalidades muy complejas o deseos, responde wishlist. "
            "Si no describe un defecto ni una funcionalidad compleja, responde el nivel más bajo."
        ),
        "criteria": [
            "wishlist: funcionalidades muy complejas o deseos",
            "low: problema estético o menor, sin impacto en el uso",
            "medium: la función falla pero existe un contorno sencillo",
            "high: una función importante falla y no hay contorno conocido",
            "critical: pérdida de datos, fallo de seguridad o servicio caído",
        ],
    },
    "area": {
        "type": "choice",
        "instructions": "¿Qué parte del proyecto está afectada según `issue`?",
        "criteria": {
            "frontend": "Interfaz de usuario, navegador, estilos, formularios",
            "backend": "API, base de datos, lógica de negocio, servidor",
            "infra": "Despliegue, CI/CD, contenedores, configuración de entorno",
            "docs": "README, guías, comentarios o ejemplos de documentación",
            "indefinido": "La issue no da pistas suficientes para decidir",
        },
    },
    "duplicada": {
        "type": "noul",
        "instructions": (
            "¿Describe `issue` el mismo problema que alguna de las "
            "`candidatas_duplicado`?"
        ),
        "criteria": {
            "true": "Mismo síntoma o misma petición que una candidata",
            "false": "Ninguna candidata describe lo mismo",
        },
    },
    "falta_info": {
        "type": "noul",
        "instructions": (
            "¿Le falta a `issue` información básica para poder reproducir "
            "o entender el problema (pasos, versión o entorno)?"
        ),
        "criteria": {
            "true": "No hay pasos, versión ni entorno; es imposible reproducirlo",
            "false": "Hay datos suficientes para empezar a investigar",
        },
    },
}


def ask_jev(state):
    r = requests.post(
        "https://openrouter.ai/api/alpha/decisions",
        headers={"Authorization": f"Bearer {os.environ['OPENROUTER_API_KEY']}"},
        json={"model": MODEL, "state": state, "questions": QUESTIONS},
        timeout=60,
    )
    r.raise_for_status()
    return r.json()["answers"]


def decide(answers):
    labels = []
    sure = True  # Se pide revisión si es False

    tipo = answers["tipo"]
    if tipo["confidence"] >= MIN_CONFIDENCE:
        labels.append(tipo["choice"])
    else:
        sure = False

    # La severidad aplica a bugs y a funcionalidades complejas o deseos (wishlist).
    sev = answers["severidad"]
    if "bug" in labels:
        if sev["confidence"] >= MIN_CONFIDENCE:
            labels.append("severity:" + SEVERITY[round(sev["score"])])
        else:
            sure = False
        if answers["falta_info"]["noul"] >= MIN_YES:
            labels.append("needs-info")
    elif "feature" in labels:
        if sev["confidence"] >= MIN_CONFIDENCE and SEVERITY[round(sev["score"])] == "wishlist":
            labels.append("severity:wishlist")

    area = answers["area"]
    if area["choice"] != "indefinido" and area["confidence"] >= MIN_CONFIDENCE:
        labels.append("area:" + area["choice"])
    else:
        sure = False

    # Solo se sugiere: la issue nunca se cierra automáticamente.
    if answers["duplicada"]["noul"] >= MIN_YES:
        labels.append("possible-duplicate")

    if not sure:
        labels.append("needs-triage")
    return labels


def crear_etiquetas(github, repo_url, labels):
    r = github.get(f"{repo_url}/labels")
    etiquetas_repo = r.json()

    nombres_existentes = []
    for e in etiquetas_repo:
        nombres_existentes.append(e["name"])

    for label in labels:
        if label in nombres_existentes:
            continue

        # Si es una etiqueta de severidad, buscamos su color y la creamos
        if label.startswith("severity:"):
            sev = label.replace("severity:", "")
            if sev in SEVERITY_COLORS:
                color = SEVERITY_COLORS[sev]
                github.post(f"{repo_url}/labels", json={"name": label, "color": color})


def main():
    with open(os.environ["GITHUB_EVENT_PATH"]) as f:
        issue = json.load(f)["issue"]

    github = requests.Session()
    github.headers["Authorization"] = f"Bearer {os.environ['GITHUB_TOKEN']}"
    repo_url = f"https://api.github.com/repos/{os.environ['GITHUB_REPOSITORY']}"
    api = f"{repo_url}/issues"

    # Jev solo evalúa lo que le mandamos, así que las candidatas a duplicado
    # (las 20 issues abiertas más recientes) van dentro del state.
    r = github.get(api, params={"per_page": 20}, timeout=30)
    r.raise_for_status()
    state = {
        "issue": {"titulo": issue["title"], "cuerpo": (issue["body"] or "")[:4000]},
        "candidatas_duplicado": [
            {"numero": i["number"], "titulo": i["title"]}
            for i in r.json()
            if i["number"] != issue["number"] and "pull_request" not in i
        ],
    }

    answers = ask_jev(state)
    labels = decide(answers)
    print(json.dumps(answers, indent=2, ensure_ascii=False))
    print("Etiquetas:", labels)

    crear_etiquetas(github, repo_url, labels)

    r = github.post(f"{api}/{issue['number']}/labels", json={"labels": labels}, timeout=30)
    r.raise_for_status()


if __name__ == "__main__":
    main()