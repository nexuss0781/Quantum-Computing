import csv
import math
import os
import sys

import matplotlib.pyplot as plt

sys.path.append(os.path.join(os.path.dirname(__file__), "../build"))
import qau_cpp

OUTPUT_DIR = os.path.dirname(__file__)


def main():
    print("=== QAU Vacuum Atomic Unit — Python Application Harness ===")
    vacuum = qau_cpp.VacuumMode.vacuum(32)
    squeezed = qau_cpp.VacuumMode.squeezed_vacuum(64, 0.8, 0.0)
    scan = squeezed.scan_quadratures(1440)
    lossy = qau_cpp.VacuumMode.lossy_variance(scan.minimum_variance, 0.90)

    assert abs(vacuum.mean_occupation()) < 1e-10
    assert abs(vacuum.mean_energy() - 0.5) < 1e-10
    assert abs(vacuum.quadrature_variance(0.0) - 0.5) < 1e-10
    assert scan.minimum_variance < 0.5
    assert lossy > scan.minimum_variance
    assert lossy < 0.5

    rows = []
    for index in range(720):
        angle = math.pi * index / 720.0
        rows.append({
            "angle": angle,
            "vacuum_variance": vacuum.quadrature_variance(angle),
            "squeezed_variance": squeezed.quadrature_variance(angle),
            "lossy_squeezed_variance": qau_cpp.VacuumMode.lossy_variance(
                squeezed.quadrature_variance(angle), 0.90
            ),
        })

    csv_path = os.path.join(OUTPUT_DIR, "vacuum_mode_sensing_scan.csv")
    with open(csv_path, "w", newline="", encoding="utf-8") as handle:
        writer = csv.DictWriter(handle, fieldnames=list(rows[0].keys()))
        writer.writeheader()
        writer.writerows(rows)

    figure, axis = plt.subplots(figsize=(10, 5.5), constrained_layout=True)
    axis.plot([row["angle"] for row in rows], [row["vacuum_variance"] for row in rows],
              label="Vacuum reference", color="#374151", linewidth=2)
    axis.plot([row["angle"] for row in rows], [row["squeezed_variance"] for row in rows],
              label="Squeezed vacuum", color="#047857", linewidth=2)
    axis.plot([row["angle"] for row in rows], [row["lossy_squeezed_variance"] for row in rows],
              label="90% transmissivity", color="#dc2626", linewidth=2, linestyle="--")
    axis.axhline(0.5, color="#1d4ed8", linestyle=":", linewidth=1.5, label="Vacuum noise floor")
    axis.set_xlabel("Homodyne quadrature angle (radians)")
    axis.set_ylabel("Dimensionless quadrature variance")
    axis.set_title("QAU Vacuum Unit: Vacuum-Referenced Quantum Sensing")
    axis.grid(alpha=0.3)
    axis.legend()
    figure.savefig(os.path.join(OUTPUT_DIR, "vacuum_mode_sensing.png"), dpi=180)
    plt.close(figure)

    print(f"vacuum_mean_energy={vacuum.mean_energy():.12f}")
    print(f"minimum_squeezed_variance={scan.minimum_variance:.12f}")
    print(f"squeezing_improvement_db={scan.improvement_db:.6f}")
    print(f"lossy_variance={lossy:.12f}")
    print("Vacuum-mode Python application harness: ALL TESTS GREEN")


if __name__ == "__main__":
    main()
