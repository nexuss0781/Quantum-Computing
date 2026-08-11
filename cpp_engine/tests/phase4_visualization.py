import csv
import os
import sys

import matplotlib.pyplot as plt

sys.path.append(os.path.join(os.path.dirname(__file__), "../build"))
import qau_cpp

OUTPUT_DIR = os.path.dirname(__file__)


def edge_lengths(geometry):
    return [float(edge.length) for edge in geometry.edges]


def draw_geometry(axis, geometry, title, color_map):
    vertices = geometry.vertices
    edges = geometry.edges
    for edge in edges:
        a = vertices[edge.a]
        b = vertices[edge.b]
        ratio = edge.length / edge.baseline_length
        axis.plot([a.x, b.x], [a.y, b.y], color=color_map(ratio), linewidth=1.5, alpha=0.75)
    scatter = axis.scatter(
        [vertex.x for vertex in vertices],
        [vertex.y for vertex in vertices],
        c=[vertex.energy_density for vertex in vertices],
        cmap="magma",
        s=42,
        edgecolor="black",
        linewidth=0.3,
        zorder=3,
    )
    axis.set_aspect("equal")
    axis.set_title(title)
    axis.set_xlabel("x")
    axis.set_ylabel("y")
    axis.grid(alpha=0.2)
    return scatter


def main():
    print("=== Project Event Horizon — Phase IV Python Integration Harness ===")
    geometry = qau_cpp.DiscreteGeometry.triangular_grid(9, 9, 1.0)
    geometry.set_radial_gaussian_source(4.0, 4.0, 2.0, 0.9)
    initial_lengths = edge_lengths(geometry)
    trajectory = geometry.evolve(10, 0.35, 0.25, 0.55)
    final_lengths = edge_lengths(geometry)

    assert all(point.metric_valid for point in trajectory)
    assert all(length > 0.0 for length in final_lengths)
    assert max(point.total_energy for point in trajectory) - min(point.total_energy for point in trajectory) < 1e-10
    assert sum(final_lengths) < sum(initial_lengths)
    assert geometry.metric_is_valid()

    rows = []
    for point in trajectory:
        rows.append({
            "step": int(point.step),
            "total_energy": float(point.total_energy),
            "average_edge_length": float(point.average_edge_length),
            "mean_abs_interior_curvature": float(point.mean_abs_interior_curvature),
            "maximum_abs_interior_curvature": float(point.maximum_abs_interior_curvature),
            "mean_ollivier_ricci": float(point.mean_ollivier_ricci),
            "maximum_edge_delta": float(point.maximum_edge_delta),
            "metric_valid": int(point.metric_valid),
        })
    csv_path = os.path.join(OUTPUT_DIR, "phase4_backreaction.csv")
    with open(csv_path, "w", newline="", encoding="utf-8") as handle:
        writer = csv.DictWriter(handle, fieldnames=list(rows[0].keys()))
        writer.writeheader()
        writer.writerows(rows)

    cmap = plt.get_cmap("viridis")
    figure, axes = plt.subplots(2, 2, figsize=(12, 10), constrained_layout=True)
    draw_geometry(axes[0, 0], geometry, "Final geometry: source-compressed edge metric", cmap)
    axes[0, 0].text(
        0.02,
        0.02,
        "Color = final length / baseline length\nNode color = source density",
        transform=axes[0, 0].transAxes,
        fontsize=9,
        bbox={"facecolor": "white", "alpha": 0.8},
    )

    steps = [row["step"] for row in rows]
    axes[0, 1].plot(steps, [row["average_edge_length"] for row in rows], marker="o", label="Average edge length")
    axes[0, 1].plot(steps, [row["mean_abs_interior_curvature"] for row in rows], marker="s", label="Mean |curvature|")
    axes[0, 1].set_title("Matter-to-geometry response")
    axes[0, 1].set_xlabel("Evolution step")
    axes[0, 1].grid(alpha=0.3)
    axes[0, 1].legend()

    axes[1, 0].plot(steps, [row["mean_ollivier_ricci"] for row in rows], marker="o", color="#b91c1c")
    axes[1, 0].set_title("Weighted transport-curvature diagnostic")
    axes[1, 0].set_xlabel("Evolution step")
    axes[1, 0].set_ylabel("Mean Ollivier-Ricci proxy")
    axes[1, 0].grid(alpha=0.3)

    axes[1, 1].plot(steps, [row["maximum_edge_delta"] for row in rows], marker="o", color="#047857")
    axes[1, 1].set_title("Relaxed update magnitude")
    axes[1, 1].set_xlabel("Evolution step")
    axes[1, 1].set_ylabel("Maximum edge-length delta")
    axes[1, 1].grid(alpha=0.3)

    figure.suptitle("Project Event Horizon — Phase IV Discrete Backreaction", fontsize=15, fontweight="bold")
    figure.savefig(os.path.join(OUTPUT_DIR, "phase4_backreaction.png"), dpi=180)
    plt.close(figure)

    print("Initial total edge length:", round(sum(initial_lengths), 8))
    print("Final total edge length:", round(sum(final_lengths), 8))
    print("Final mean absolute curvature:", round(rows[-1]["mean_abs_interior_curvature"], 8))
    print("Final mean transport curvature:", round(rows[-1]["mean_ollivier_ricci"], 8))
    print("Phase IV Python integration harness: ALL TESTS GREEN")


if __name__ == "__main__":
    main()
