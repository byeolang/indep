# Changelog

All notable changes to this repository will be documented in this file.

## v0.1.5
+ update latest patch for build-common (windows build compatibility)

## v0.1.4
- fixed build error on Windows with the latest MSVC toolchain.
- `builder.py` now supports `doc` and `pubdoc` commands for generating and publishing documentation.

## v0.1.3
- build error occurs when stela is called to FetchContent by host project
- host project can't call FetchContent to this because header files of stela aren't exposed to the public.

## v0.1.2
- dependency checking has been added by using build-common cmake function.

## v0.1.1
- fully working `stela` repository, with all features and functionality from the pre-split history.

## Pre-split history

Before this repository was created, stela-related development happened inside the main `byeol` repository.

That earlier history is not reproduced here in full.
Please refer to the `byeol` repository for detailed pre-split changelog entries, design context, and implementation history.

Selected milestones related to stela before the split:

- Introduced the configuration / manifest layer used by the byeol ecosystem.
- Expanded its role in pack and metadata handling.
- Established its direction as a distinct layer worth separating into its own repository.

## v0.1.0
+ Initial standalone `stela` repository.
+ Initial repository layout, build setup, and documentation.
