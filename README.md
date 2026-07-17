# indep

**indep** is a small collection of platform-independent utilities shared across the all core byeol projects.

The project is designed to provide lightweight, reusable components with minimal dependencies. 
It serves as a common foundation for multiple repositories while avoiding unnecessary coupling to higher-level modules.

## Goals

* Keep the implementation platform-independent whenever possible.
* Minimize external dependencies.
* Provide reusable building blocks for other projects.
* Maintain a clean and stable API.
* Focus on utility code rather than domain-specific logic.

## Design Principles

### Independent

The library should not depend on other byeol components unless absolutely necessary. It is intended to remain reusable in a wide variety of projects.

### Lightweight

Only include functionality that is broadly useful. Features that are highly specialized belong in higher-level libraries instead.

### Portable

All code should compile across supported platforms without platform-specific assumptions. Platform-dependent implementations should be isolated behind well-defined interfaces.

### Maintainable

Prefer simple implementations over clever ones. Long-term readability is valued more than micro-optimizations.

## Scope

Typical contents of this library may include:

* Platform abstraction helpers
* File system utilities
* String utilities
* Time and date utilities
* Environment helpers
* Common algorithms
* Small reusable data structures
* Miscellaneous helper functions

The exact contents may evolve over time as the byeol ecosystem grows.

## Relationship to Other Projects

`indep` is one of the foundational libraries of the byeol ecosystem.

Higher-level components may depend on **indep**, but **indep** should avoid depending on them. This one-way dependency helps keep the overall architecture clean and modular.

## Development Guidelines

When adding new functionality, consider the following questions:

* Is this feature generally useful?
* Can it be implemented without introducing heavy dependencies?
* Does it belong in a lower-level utility library?
* Will it remain platform-independent?
* Does it improve the ecosystem as a whole rather than a single project?

If the answer to most of these questions is **no**, the feature probably belongs in another module instead.

## License

This project is licensed under the same license as the byeol project.
