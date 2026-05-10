# ∆ Qastilog

# Este es un proyecto para que los creadores de proyectos app o web puedan tener transparencia colectiva con sus usuarios...
## Actualmente solo contamos con changelog, aunque se vienen nuevas actualizaciones.

---

## Cómo añadir una nueva empresa

### Paso 1 — Crea el archivo de datos

Crea un archivo .json con esta estructura:

```json
{
  "companyName": "Nueva Empresa",
  "slug": "nueva-empresa",
  "logo": "https://cdn.example.com/logo.png",
  "website": "https://nuevaempresa.com",
  "entries": [
    {
      "version": "1.0.0",
      "date": "2025-05-01",
      "title": "Lanzamiento inicial",
      "changes": [
        "Primera versión pública.",
        "Soporte para autenticación con Google.",
        "Dashboard inicial con métricas básicas."
      ]
    }
  ]
}
```

**Campos obligatorios:** `companyName`, `slug`, `entries`.  
**Campos opcionales:** `logo` (URL de imagen), `website` (URL del sitio).

### Paso 2 — Haz un correo a ewoetios@gmail.com para que te puedan añadir.

---

## Cómo añadir una entrada de changelog

Abre el .json y agrega un objeto al principio del array `entries`:

```json
{
  "version": "2.1.0",
  "date": "2025-06-15",
  "title": "Nuevas integraciones",
  "changes": [
    "Integración con Zapier y Make (antes Integromat).",
    "**Nuevo** sistema de notificaciones por correo.",
    "Corrección de *bug* en la exportación de PDF."
  ]
}
```

Las entradas se ordenan automáticamente por fecha (las más recientes primero).

### Formato de texto en `changes`

El campo `changes` acepta **Markdown ligero**:

| Sintaxis        | Resultado     |
|-----------------|---------------|
| `**texto**`     | **negrita**   |
| `*texto*`       | *cursiva*     |
| `` `código` ``  | `código`      |

---

## Estructura del JSON de empresa

| Campo         | Tipo     | Obligatorio | Descripción                          |
|---------------|----------|-------------|--------------------------------------|
| `companyName` | string   | ✅          | Nombre completo de la empresa        |
| `slug`        | string   | ✅          | Identificador URL (sin espacios)     |
| `logo`        | string   | ❌          | URL absoluta a la imagen del logo    |
| `website`     | string   | ❌          | URL del sitio web de la empresa      |
| `entries`     | array    | ✅          | Lista de entradas de changelog       |

### Estructura de cada entrada (`entries[]`)

| Campo     | Tipo     | Obligatorio | Descripción                           |
|-----------|----------|-------------|---------------------------------------|
| `version` | string   | ✅          | Número de versión (ej. `"2.1.0"`)    |
| `date`    | string   | ✅          | Fecha ISO 8601 (`"YYYY-MM-DD-HH"`)   |
| `title`   | string   | ✅          | Título descriptivo del release       |
| `changes` | string[] | ✅          | Array de cambios (acepta Markdown)   |

---

## Licencia

MIT — úsalo y modifícalo libremente.

-- Notas:
> Se usa Claude para mantener el proyecto, aunque después se realizan cambios manuales
