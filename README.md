# stela
`stela` is a separate repository for the configuration / manifest layer used by the byeol project.

Although this repository is managed independently, its design, goals, and long-term direction are still closely tied to the overall byeol ecosystem.

---

## Overview

stela exists to separate and organize the parts of the byeol ecosystem that deal with:

- configuration and manifest representation
 pack / toolchain / FFI-related metadata
- parser / loader infrastructure for structured project metadata
- independent maintenance of the configuration layer

In other words, stela is not just a helper file format.  
It is intended to be the structured configuration and manifest layer of the wider byeol ecosystem.

---

## Current Status

This repository is still in an early stage of separation.

That means the following may continue to evolve:

- syntax details
- internal directory layout
- build structure
- public API and usage patterns
- manifest and FFI-related specifications

At this stage, it should be understood less as a fully standalone product and more as a repository extracted from the broader byeol project for better separation of concerns.

---

## Important

This repository does **not** currently contain the full background needed to understand every design decision in isolation.

For the broader context, you should refer to the main **byeol** repository, especially for:

- project philosophy
- overall architecture
- pack / manifest model
- FFI direction
- contribution workflow
- shared build conventions
- terminology and design rationale

So while `stela` is now a separate repository, **the main conceptual and architectural context still lives in byeol**.

---

## Recommended Reading Order

If you are new to this repository, it is recommended that you read materials in roughly this order:

1. High-level documentation in the main `byeol` repository
2. `byeol` onboarding / architecture / contribution documents
3. Then the code and documents in this repository

Starting from `stela` alone may make some of the design choices look arbitrary, when they are actually derived from the larger byeol direction.

---

## Build / Integration

This repository is expected to follow the shared build conventions used across byeol-related projects.

Depending on the stage of development, it may be used in one or more of the following ways:

- built independently
- included by a parent project through `FetchContent` or `add_subdirectory`
- consumed indirectly through a larger toolchain or packaging flow

More detailed build instructions may be added as the repository structure stabilizes.

---

## Contributing

Contributions are welcome, but large changes should be made with awareness of the wider byeol project direction.

In particular, the following kinds of changes should be considered in the context of the main byeol repository as well:

- syntax additions or changes
- manifest schema changes
- FFI-related representation changes
- public interface changes
- build or packaging policy changes

In short: changes here may have implications outside this repository.

---

## Why This Repository Exists

This repository was split out in order to improve:

- separation of responsibilities
- clarity of project structure
- potential for independent versioning
- reuse across related repositories
- maintainability over time

That said, separation does not imply complete independence.  
For now, the most accurate way to understand `stela` is as a focused repository within the larger byeol ecosystem.

---

## Summary

- `stela` is the configuration / manifest layer repository for the byeol ecosystem.
- It is still in an early stage and parts of its structure and specification may change.
- For the full design rationale and project context, you should also read the main **byeol** repository.
