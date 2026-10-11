# KATA3 · Scene Compiler Foundation v0.1

A dependency-free C++20 scene language prototype. Structural constraints reduce accidental drift; they **do not** guarantee literary quality or verify AI-generated content.

## Build

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Grammar v0.1

```
SCENE midnight
WORLD = HAKUI
LOCATION = DEFACTO_TOWER
MOOD = NOCTURNAL
PRESERVE_CANON = TRUE
PLAYER_AUTONOMY = TRUE
ACTOR ether PLAYER SEEK_TRUTH
ACTOR stranger NPC HIDE_SECRET
END
```

Each field is single-assignment, actor identifiers are unique, and the two invariants must be explicitly `TRUE`. The parser returns line-numbered diagnostics. No scene generation, model adapter, simulation, or serialization is implied.

## Narrative sidecar v0.2

The base `SCENE` language remains unchanged. Call `parse_narrative(source, scene)` after parsing a valid scene. It enforces player agency structurally but does not generate text, decide choices or execute consequences.

```text
OBJECTIVE discover | Find out why the tower is closed
DIALOGUE stranger | The doors will not open.
CHOICE ask | Ask for a key | EVENT:ask_key
CHOICE leave | Walk away | EVENT:leave_tower
```

Rules: one OBJECTIVE, at least two uniquely identified CHOICE directives, dialogue actors must exist in the scene, and every choice declares a symbolic EVENT consequence. The scene must contain a PLAYER actor and enable both canon and autonomy laws. A consequence is a proposal for a future simulation adapter, not a state mutation. Tests reject malformed choices, duplicate IDs, missing player agency and unknown speakers. The grammar intentionally reserves `|` as the field delimiter; escaping and richer storytelling semantics are future work.

Build with CMake and run CTest in Debug or Release. The `kata3.narrative` target is independent of model providers or network access.
