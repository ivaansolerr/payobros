"""Triaje de issues con Jev.

"""

import json
import os

import requests

# Versión fijada: el alias "latest" podría cambiar las respuestas sin avisar.
MODEL = "typesafe/jev-1.13"

MIN_CONFIDENCE = 0.6  # choice/score: por debajo no se etiqueta y se pide revisión
MIN_YES = 0.8  # noul: probabilidad de "sí" a partir de la cual se etiqueta
SEVERITY = ["wishlist", "low", "medium", "high", "critical"]  # el índice es el valor del score

# COLORES
COLOR_MORADO = "6f42c1"      # severity:wishlist
COLOR_VERDE = "0e8a16"       # severity:low
COLOR_AMARILLO = "fbca04"    # severity:medium
COLOR_NARANJA = "d93f0b"     # severity:high
COLOR_ROJO = "b60205"        # severity:critical
COLOR_AZUL = "1d76db"        # rol:motor
COLOR_ROSA = "e875c2"        # rol:jugador
COLOR_CYAN = "0dceda"        # rol:combate
COLOR_VERDE_ROL = "2ea44f"   # rol:niveles
COLOR_VIOLETA = "5319e7"     # rol:ia
COLOR_GRIS = "bfdadc"        # rol:global

SEVERITY_COLORS = {
    "wishlist": COLOR_MORADO,
    "low": COLOR_VERDE,
    "medium": COLOR_AMARILLO,
    "high": COLOR_NARANJA,
    "critical": COLOR_ROJO,
}

ROLE_COLORS = {
    "motor": COLOR_AZUL,
    "jugador": COLOR_ROSA,
    "combate": COLOR_CYAN,
    "niveles": COLOR_VERDE_ROL,
    "ia": COLOR_VIOLETA,
    "global": COLOR_GRIS,
}

# ASIGNACIÓN DE ROLES A MIEMBROS DEL EQUIPO (GitHub usernames)
ROLE_ASSIGNEES = {
    "motor": ["Arid3n"],
    "jugador": ["ivaansolerr"],
    "combate": ["jrcb3-ua"],
    "niveles": ["joaquinavarro"],
    "ia": ["jvev1"],
    "global": ["Arid3n", "ivaansolerr", "jrcb3-ua", "joaquinavarro", "jvev1"],
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
    "rol": {
        "type": "choice",
        "instructions": (
            "¿Qué miembro del equipo debería encargarse de la issue descrita "
            "en `issue`? Elige el rol cuya área de responsabilidad encaje mejor "
            "con el problema o la petición."
            "Si no estás seguro o requiere una revisión de todo el grupo, responde `global`"
        ),
        "criteria": {
            "motor": (
                "Afecta al motor del juego: StateMachine, GameState, main.cpp, "
                "CMakeLists.txt, transiciones entre estados (menú, pausa, victoria, "
                "game over, selección de nivel) o configuración general del juego"
            ),
            "jugador": (
                "Afecta a las mecánicas del jugador: Player, movimiento, salto, "
                "gravedad, input, sistema de vidas/salud, puntuación, HUD, "
                "apuntado con ratón o multijugador local"
            ),
            "combate": (
                "Afecta al sistema de combate: Weapon, Projectile, tipos de "
                "proyectil, disparo, recarga, munición, sistema de daño, pickups "
                "de armas o inventario de armas"
            ),
            "niveles": (
                "Afecta a los niveles o las físicas: Level, carga de mapas .tmx, "
                "plataformas, Box2D, PhysicsUnits, colisiones con el mundo, "
                "cámara, plataformas móviles u objetos interactivos del mapa"
            ),
            "ia": (
                "Afecta al comportamiento de los enemigos o NPCs: Enemy, IA, "
                "patrullaje, persecución, ataque, tipos de enemigos, spawner "
                "de enemigos o colisiones enemigo-mundo"
            ),
            "global": (
                "No encaja claramente en un solo rol, afecta a varias áreas "
                "a la vez, o no hay información suficiente para decidir"
            ),
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

    # Asignación por rol del equipo.
    rol = answers["rol"]
    if rol["choice"] != "global" and rol["confidence"] >= MIN_CONFIDENCE:
        labels.append("rol:" + rol["choice"])
    else:
        labels.append("rol:global")

    if not sure:
        labels.append("needs-triage")
    return labels


def asignaciones_rol(labels):
    """Devuelve la lista de GitHub usernames a asignar según la etiqueta de rol."""
    rol_encontrado = ""

    for label in labels:
        if label.startswith("rol:"):
            rol_encontrado = label.replace("rol:", "")
            break

    if rol_encontrado in ROLE_ASSIGNEES:
        assignees = ROLE_ASSIGNEES[rol_encontrado]
    else:
        assignees = []
    return assignees


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

        # Si es una etiqueta de rol, buscamos su color y la creamos
        if label.startswith("rol:"):
            rol = label.replace("rol:", "")
            if rol in ROLE_COLORS:
                color = ROLE_COLORS[rol]
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

    # Asignar la issue al miembro (o miembros) del equipo según el rol.
    assignees = asignaciones_rol(labels)
    if assignees:
        r = github.post(
            f"{api}/{issue['number']}/assignees",
            json={"assignees": assignees},
            timeout=30,
        )
        r.raise_for_status()
        print("Asignada a:", assignees)


if __name__ == "__main__":
    main()