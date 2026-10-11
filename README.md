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

## Provider-neutral scene generation bridge v0.3

`compile_generation_request(scene, approved_narrative)` produces a text-generation instruction containing the scene's actors, fixed objective and approved choices. No model API is called by this library.

`review_generation(response, scene, approved_narrative)` parses the proposed text using the C2 narrative parser and enforces:
- Existing scene must preserve canon and player autonomy.
- Model output must preserve the exact objective and the **ordered** choice list, including all labels and symbolic event identifiers.
- Dialogue speakers must be declared actors and must not be player actors.
- Unknown directives, missing choices and oversized responses (over 64 KiB) are rejected.
- Review has no side effects and cannot execute `EVENT:` or infer player action.

This is **structural enforcement**, not proof of semantic canon consistency, prompt-injection security, artistic quality or model compliance. A caller should check `GenerationReview::accepted` before displaying or storing output.

Run with `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build --output-on-failure`.

## C4 — deterministic choice ledger (v0.4)

Include `<kata3/runtime.hpp>`. `NarrativeRuntime::select(scene, approved_narrative, state, choice_id, expected_sequence)` requires an **explicit player-selected choice ID**. It appends one symbolic `ChoiceEvent` only when the choice is approved, the caller's expected history length matches, the existing ledger is valid and both canon and autonomy laws are enabled.

`NarrativeRuntime::replay(scene, approved_narrative, events)` rebuilds the ledger by checking every event's sequence, selected choice and event identifier against the approved narrative. Invalid histories are rejected without modifying the caller's state.

**Authority boundary:** a `ChoiceEvent` is a declared narrative consequence, **not an applied world-state mutation**. KATA3 does not select player actions, call models, or write to HAKUI/SpiralOS HOME. Replay assumes an immutable approved narrative; cross-version migrations, persistent storage, cryptographic audit proofs and external world-state dispatch remain outside this release.

Run `cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build --output-on-failure`. The C4 test target is `kata3.runtime`.

## C4.2 — Persistent narrative memory (v0.5.0)

Header-only `NarrativeStorage` in `include/kata3/storage.hpp` writes a bounded (≤ 2 MiB) `KATA3_LEDGER_V1` snapshot with little-endian event records and a noncryptographic FNV-1a checksum. Storage is scoped to the approved scene ID and objective ID, and replays each choice against the supplied narrative contract on load. A failed decode or load does not replace the caller's current state.

`save(path, scene, narrative, state)` writes to a sibling `.pending` path and renames it into place; callers must ensure single-writer access. These operations are **not** guaranteed atomic or durable on all filesystems, especially Windows replacement and power loss. Symlink checks are not race-free. The checksum detects accidental corruption, not malicious tampering. Storage does not choose player actions or mutate SPIRAL-OS-HOME/HAKUI state.

`tests/storage_tests.cpp` checks roundtrip, truncation, checksum failure, wrong scene/contract, rejected forged events and preserved destination state on load failure. Cross-platform CI verification and interrupted-write recovery remain open.

## V1 — Playable console vertical slice (v0.6)

Build using CMake and launch `kata3_play` with three positional paths: the scene file, narrative file, and writable save file. For example, from the project root after building:

```sh
./build/kata3_play examples/dock.scene.k3 examples/dock.narrative.k3 my-dock.k3save
```

On Windows, the executable may be located under `build/Release/kata3_play.exe`.

The console prints the scene, objective, NPC dialogue, and available options, then reads **one explicitly player-selected choice ID**. `quit` exits without modifying the ledger. A valid choice is persisted via `NarrativeStorage`. Restart with the same save path to see the recovered history count and choose again. Invalid selections and invalid pre-existing saves return nonzero without overwriting the save. Saved `EVENT:` strings are symbolic records only; HAKUI and SPIRAL-OS-HOME are never modified.

Automated regression target: `kata3.console`, which tests two sessions, restore after restart, invalid selection and preserved history. Filesystem replacement and crash durability still depend on platform and require follow-up hardening.
