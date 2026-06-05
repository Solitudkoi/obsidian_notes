## Objective

Welcome to this assignment! In computer-aided design (CAD) and parametric modeling, a sketch is rarely a static drawing. Designers specify _relationships_—coincident points, fixed lengths, parallel edges, tangent arcs—and expect the geometry to _update_ when dimensions change. That workflow is **parametric design**: geometric entities are driven by numerical parameters, and a **geometric constraint solver (GCS)** adjusts those parameters so every relationship is satisfied at once.

Over Homeworks 1–6 you built the pieces of such a system: a 2D sketch kernel, automatic differentiation, templated linear algebra, a Newton-based nonlinear solver, and a test harness. **This assignment assembles them into a GCS.** You will refactor the sketch around a centralized parameter store, implement geometric constraints as differentiable error equations, and customize a Newton solver to drive the sketch toward a feasible configuration.

This module will test your understanding of:

- **Inheritance & Polymorphism:** Extending a geometry hierarchy (`GeometricEntity` -> `Vertex` / `Edge` -> `Line`, `Circle`, `Arc`) and a family of `Constraint` subclasses that share behavior through virtual methods.
- **Abstract Interfaces:** Implementing pure and defaulted virtual hooks (`define_constraints`, `get_geometric_dependencies`, `get_owned_parameters`, `get_owned_constants`) so each constraint type plugs into the same framework.
- **Design Patterns:** Applying the _Template Method_ pattern via `SketchSolver` / `NewtonSolver`, and using `Sketch` as a facade that owns entities and exposes factory-style `add_*` methods for constraints.
- **Composition & Object Graphs:** Wiring constraints to the geometric objects they depend on, tracking dependents for safe deletion, and separating owned geometric parameters from user-specified constants across two managers.
- **Refactoring/Integration in a Growing Codebase:** Porting solutions from earlier homeworks into a larger project and adapting them to new base classes, renamed methods, and ownership rules without breaking callers.

## The Skeleton Code

Your task is to search for `[todo]` comments and _complete the implementation at those marked sites_. Before writing code, skim how the main pieces fit together:

- `ParameterManager` — stores geometric unknowns (vertex coordinates, circle radius, …) keyed by `ParamId`.
- `GeometricEntity` / `Vertex` / `Edge` / `Line` / `Circle` / `Arc` — sketch geometry; vertices and circles _own_ parameter IDs instead of inline `double` values.
- `Constraint` — builds an autodiff graph per constraint, exposes errors and Jacobian rows, and registers with geometric dependencies.
- `Sketch` — manages vertices, edges, constraints, and two parameter stores (`param_manager_` for unknowns of sketch geometry, `const_manager_` for user-specified targets such as angles or distances).
- `SketchSolver` — subclasses your `NewtonSolver` to solve the stacked constraint system.

**Porting from Homeworks 1–6**:

- **HW2 (`nanotest/`):** Copy the testing framework; remember to incorporate the added testing macros from Homework 4/5.
- **HW3 (`geometry/`):** Copy geometry classes; adapt `Edge` to inherit `GeometricEntity`, rename dependency accessors (`get_vertex_dependencies`, `add_edge_dependent`, ...), and wire vertex coordinates and circle radius through `ParameterManager`.
- **HW4 (`autodiff/`):** Copy AutoDiff; apply skeleton changes (dirty flag to skip redundant forward passes, smart pointers to avoid manual cleanup).
- **HW5 (`linalg/`):** Copy templated linear algebra.
- **HW6 (`nlopt/`):** Copy templated `NewtonSolver`.

**New work for Homework 7** (compact summary—details are in comments and in reference constraints such as `Horizontal`, `Parallel`, `PointPointCoincidence`):

- **`param/`:** Implement `ParameterManager` CRUD (`create_param`, `delete_param`, `get_value`, `set_value`, enumerating IDs).
- **`geometry/`:** Vertex/circle owned parameters and accessors; `GeometricEntity` constraint-dependents and `get_owned_parameters()`.
- **`constraint/`:** Base class hooks (`define_constraints`, constant accessors, dependency register/unregister); finish stubbed types (`Vertical`, `Perpendicular`, `PointOnLine`, `PointToPointDistance`, `EqualDistance`, `EqualRadius`, `ThreePointsAngle`, arc tangency helpers).
- **`sketch.*`:** Accessors; smart pointers for entity ownership; factory methods for distance/vertical/perpendicular constraints; edge-deletion cleanup; JSON segment for edges; remaining SVG viewbox branch.
- **`sketch_solver.*`:** `initialize`, `evaluate_errors`, `evaluate_jacobian`, `apply_parameter_updates` for the Newton template method.

Several constraints are already fully implemented in the skeleton—study them before implementing new ones.

## Test Sample

To test your implementation, we provide two test cases in `src/main.cpp` (see the following figure). You can build and test by running `main.sh`.

![Description](https://images.ptausercontent.com/0ba6ecde-f0e0-4cfa-a3f8-23ed90eb2c63.svg)

## Optional Tasks (not included in the score)

To fully liberate your creativity, we also provide a web-based sketch pad at `src/optional/nano_sketch` that allows you to interactively design and solve sketches in the browser (see the following animation). To be specific, you need to implement the following tasks (marked `[optional]` in the code):

- **JSON deserialization:** Implement `Sketch::from_json`, `load_from_json`, and related parsers marked `[optional]` in `sketch.cpp` / `sketch.h`.
- **WebAssembly sketch solver:** Build `src/optional/bindings.cpp` to expose your C++ GCS module as the backend to a small frontend demo of parametric sketching in the browser. See `src/optional/README.md` for more details.

![web-based_sketch_pad_compressed.gif](https://images.ptausercontent.com/6217fd18-b09c-480c-b63c-8cb27f800566.gif)

## Design Discussion

For those interested in software architecture and CAD kernels, consider these choices:

**1. Why a `ParameterManager` instead of `double x, y` in each `Vertex`?**  
Parametric CAD separates topology (what connects to what) from configuration (numeric values). Central parameters give the solver a uniform vector of unknowns, stable IDs for serialization, and clean deletion when entities are removed.

**2. Two managers: `param_manager_` vs `const_manager_`**  
Coordinates and radii are updated by Newton steps. User-specified targets (a 45° angle, a 10 mm length) still live in the autodiff graph but must not move during `apply_parameter_updates`. Splitting managers makes that distinction explicit.

**3. Constraints as autodiff graphs**  
Hand-derived Jacobians for many constraint types do not scale. Building each residual in your Homework 4 graph lets `get_derivatives()` feed the same machinery the optimizer expects; `SketchSolver` only stacks rows—how commercial sketch kernels compose many small equations into one nonlinear system.

**4. Dependency registration**  
Constraints register on every `GeometricEntity` they reference. When an edge or vertex is deleted, constraint dependents must be found, unregistered, and owned constants released—otherwise stale pointers and orphan equations corrupt the solve.

**5. Reusing `NewtonSolver` from Homework 6**  
The sketch _is_ a nonlinear system E(P)=0 in parameter vector P. Subclassing `NewtonSolver` with sketch-specific error, Jacobian, and update hooks is the Template Method pattern: iteration, line search, and linear solve stay generic; the “physics” is sketch-specific.

