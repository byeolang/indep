# For contributors (including AI)

The entry point is the byeol repository's
[CONTRIBUTING.md](https://github.com/byeolang/byeol/blob/main/CONTRIBUTING.md).
Read it first.

It carries:
- The routing table to every detailed document on byeol.io (architecture,
  coding rules, build structure, deployment, testing, document conventions)
- The repository layout (six repos and the direction of dependencies)
- Commit and pull request conventions
- Setup commands

`indep` is one of those six repositories: the platform abstraction layer
that everything else depends on, and the only place where `#ifdef` is
allowed. When you need deeper information on a specific topic, follow the
link in the routing table rather than looking for a local Markdown file —
the detailed documents live on byeol.io, generated from `byeol`'s `doc/`.
