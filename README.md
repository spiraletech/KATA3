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
