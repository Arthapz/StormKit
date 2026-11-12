# AGENTS.md

Guidelines for AI agents and contributors working in this repository.

## Golden rule: never edit source files directly

**An AI agent must never write to, rename, move, or delete any source file.**
Read them freely — only never
modify them in place.

All proposed changes go to the dedicated folder **`proposals/`** instead:

1. Write your change as a self-contained artifact under `proposals/`:
   - a unified diff (`proposals/<short-description>.patch`) applying against the current
     sources, **or**
   - a complete replacement file at the mirrored path
     (`proposals/driver/src/hid/foo_commands.cppm`), **or**
   - a short design note (`proposals/<short-description>.md`) when the change is not yet
     concrete enough for a patch.
2. Prefix the file name with the date or a short slug so parallel proposals do not collide
   (`2026-10-19-vibration-sample-0x02.patch`).
3. In your final message, list the files you created under `proposals/` and what the user
   must do to apply them (e.g. `git apply proposals/xxx.patch`, or copy file X over Y).
4. **Wait.** Do not apply the patch yourself, do not run a build to "verify" a change you
   made to the real tree, and do not touch `build/`, `.xmake/`, `packages/`.

Only the human user applies a proposal to the real sources. After they have applied it,
they may ask you to review the result — reviewing and reading remain allowed at all times.

Everything below documents the conventions your proposals must follow so that they can be
applied as-is.

## Formatting

- C++: `clang-format` with the repository `.clang-format` (LLVM base, 4-space indent,
  120 columns, aligned consecutive declarations/assignments/trailing comments,
  `InsertBraces: false`, aligned array-of-struct literals). Run it before committing.
- Byte arrays / descriptors may use `// clang-format off/on` blocks — preserve them.
- `.gitattributes` maps `.cppm` / `.mpp` / `.ixx` to C++ for Linguist.

## Before you finish

1. Your work lives **only** under `proposals/`.
2. Any `.cpp`/`.cppm`/`.hpp` content you produce must already be `clang-format`-clean
   (`.clang-format`: LLVM base, 4-space indent, 120 columns) — you cannot run the formatter
   on the real tree, so match the style by hand.
3. Do not commit anything under `build/`, `.xmake/`, `packages/`, `.clice/`.
4. Report clearly: which proposal files you created, which source files they affect once
   applied, and what you could not verify (no test suite exists yet, and you cannot build
   unapplied changes).
