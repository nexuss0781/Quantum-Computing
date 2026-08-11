import csv
import math
import os
import sys

import matplotlib.pyplot as plt

sys.path.append(os.path.join(os.path.dirname(__file__), "../build"))
import qau_cpp

OUTPUT_DIR = os.path.dirname(__file__)


def run_otoc_scan():
    rows = []
    for layers in range(0, 9):
        state = qau_cpp.DenseQuantumState(4)
        circuit = qau_cpp.QuantumCircuit.brickwork_scrambler(4, layers, 0xC0FFEE)
        result = qau_cpp.OTOCAnalyzer.evaluate(state, circuit, 3, "Z", 0, "X")
        rows.append({
            "layers": layers,
            "otoc_real": float(result.correlator.real),
            "otoc_imag": float(result.correlator.imag),
            "commutator_squared": float(result.commutator_squared),
            "norm_error": float(result.norm_error),
        })
    return rows


def run_page_curve():
    model = qau_cpp.EvaporatingBlackHoleToyModel(6, 0xBADC0DE, 4)
    model.prepare_seeded_initial_state(8)
    points = model.evaporate_all()
    rows = []
    for point in points:
        rows.append({
            "evaporation_step": int(point.evaporation_step),
            "black_hole_qubits": int(point.black_hole_qubits),
            "radiation_qubits": int(point.radiation_qubits),
            "radiation_entropy": float(point.radiation_entropy),
            "radiation_purity": float(point.radiation_purity),
            "global_norm_error": float(point.global_norm_error),
        })
    return rows


def write_csv(path, rows):
    with open(path, "w", newline="", encoding="utf-8") as handle:
        writer = csv.DictWriter(handle, fieldnames=list(rows[0].keys()))
        writer.writeheader()
        writer.writerows(rows)


def create_figure(otoc_rows, page_rows):
    figure, axes = plt.subplots(1, 2, figsize=(13, 5), constrained_layout=True)

    layers = [row["layers"] for row in otoc_rows]
    commutators = [row["commutator_squared"] for row in otoc_rows]
    axes[0].plot(layers, commutators, marker="o", color="#7c3aed", linewidth=2)
    axes[0].set_title("Finite-Circuit OTOC Diagnostic")
    axes[0].set_xlabel("Scrambling layers")
    axes[0].set_ylabel(r"$C(t)=2(1-Re F(t))$")
    axes[0].grid(alpha=0.3)

    steps = [row["evaporation_step"] for row in page_rows]
    entropy = [row["radiation_entropy"] for row in page_rows]
    purity = [row["radiation_purity"] for row in page_rows]
    axes[1].plot(steps, entropy, marker="o", color="#dc2626", linewidth=2, label="Radiation entropy")
    axes[1].set_title("Closed-Unitary Page-Curve Toy Model")
    axes[1].set_xlabel("Evaporation step")
    axes[1].set_ylabel("Von Neumann entropy")
    axes[1].grid(alpha=0.3)
    secondary = axes[1].twinx()
    secondary.plot(steps, purity, marker="s", linestyle="--", color="#2563eb", label="Radiation purity")
    secondary.set_ylabel("Radiation purity")
    secondary.set_ylim(0.0, 1.05)

    figure.suptitle("Project Event Horizon — Phase III Diagnostics", fontsize=14, fontweight="bold")
    figure.savefig(os.path.join(OUTPUT_DIR, "phase3_otoc_page_curve.png"), dpi=180)
    plt.close(figure)


def main():
    print("=== Project Event Horizon — Phase III Python Integration Harness ===")
    otoc_rows = run_otoc_scan()
    page_rows = run_page_curve()

    assert abs(otoc_rows[0]["otoc_real"] - 1.0) < 1e-10
    assert max(row["norm_error"] for row in otoc_rows) < 1e-10
    peak_index = max(range(len(page_rows)), key=lambda index: page_rows[index]["radiation_entropy"])
    assert 0 < peak_index < len(page_rows) - 1
    assert abs(page_rows[-1]["radiation_entropy"]) < 1e-10
    assert max(row["global_norm_error"] for row in page_rows) < 1e-10

    write_csv(os.path.join(OUTPUT_DIR, "phase3_otoc.csv"), otoc_rows)
    write_csv(os.path.join(OUTPUT_DIR, "phase3_page_curve.csv"), page_rows)
    create_figure(otoc_rows, page_rows)

    print("OTOC commutator diagnostics:", [round(row["commutator_squared"], 6) for row in otoc_rows])
    print("Page-curve radiation entropy:", [round(row["radiation_entropy"], 6) for row in page_rows])
    print("Python integration and visualization harness: ALL TESTS GREEN")


if __name__ == "__main__":
    main()
