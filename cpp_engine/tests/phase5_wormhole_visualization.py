import csv
import math
import os
import sys

import matplotlib.pyplot as plt

sys.path.append(os.path.join(os.path.dirname(__file__), "../build"))
import qau_cpp

OUTPUT_DIR = os.path.dirname(__file__)


def message_label(index):
    return ["|0>", "|1>", "|+>", "|->", "|+i>", "|-i>"][index]


def run_protocols():
    protocols = [
        ("Entanglement + correction", True, True),
        ("Entanglement only", True, False),
        ("Correction only", False, True),
        ("Neither resource", False, False),
    ]
    ensemble = qau_cpp.EREPRWormholeToy.six_state_ensemble()
    rows = []
    for name, entangled, correction in protocols:
        for index, message in enumerate(ensemble):
            outcome = qau_cpp.EREPRWormholeToy.transfer(message, entangled, correction)
            rows.append(
                {
                    "protocol": name,
                    "message": message_label(index),
                    "carriers_entangled": int(entangled),
                    "coherent_feedforward": int(correction),
                    "carrier_bell_fidelity": float(outcome.carrier_bell_fidelity),
                    "receiver_fidelity": float(outcome.receiver_fidelity),
                    "receiver_purity": float(outcome.receiver_purity),
                    "global_norm_error": float(outcome.global_norm_error),
                    "bloch_x": float(outcome.receiver_bloch[0]),
                    "bloch_y": float(outcome.receiver_bloch[1]),
                    "bloch_z": float(outcome.receiver_bloch[2]),
                }
            )
    return rows


def main():
    print("=== Project Event Horizon — Phase V Python Integration Harness ===")
    rows = run_protocols()
    csv_path = os.path.join(OUTPUT_DIR, "phase5_wormhole_transfer.csv")
    with open(csv_path, "w", newline="", encoding="utf-8") as handle:
        writer = csv.DictWriter(handle, fieldnames=list(rows[0].keys()))
        writer.writeheader()
        writer.writerows(rows)

    protocol_order = ["Entanglement + correction", "Entanglement only", "Correction only", "Neither resource"]
    averages = {}
    for protocol in protocol_order:
        values = [row["receiver_fidelity"] for row in rows if row["protocol"] == protocol]
        averages[protocol] = sum(values) / len(values)

    assert abs(averages["Entanglement + correction"] - 1.0) < 1e-10
    assert averages["Entanglement only"] < 0.75
    assert averages["Correction only"] < 0.80
    assert all(row["global_norm_error"] < 1e-10 for row in rows)

    ideal_rows = [row for row in rows if row["protocol"] == "Entanglement + correction"]
    no_correction_rows = [row for row in rows if row["protocol"] == "Entanglement only"]

    figure, axes = plt.subplots(1, 2, figsize=(13, 5), constrained_layout=True)
    colors = ["#047857", "#dc2626", "#d97706", "#6b7280"]
    axes[0].bar(range(len(protocol_order)), [averages[p] for p in protocol_order], color=colors)
    axes[0].axhline(2.0 / 3.0, color="#1d4ed8", linestyle="--", linewidth=1.2, label="Classical benchmark: 2/3")
    axes[0].set_xticks(range(len(protocol_order)), ["EPR +\ncorrection", "EPR only", "Correction\nonly", "Neither"])
    axes[0].set_ylim(0.0, 1.05)
    axes[0].set_ylabel("Six-state average receiver fidelity")
    axes[0].set_title("Protocol resource controls")
    axes[0].legend(loc="lower left")
    axes[0].grid(axis="y", alpha=0.3)

    labels = [row["message"] for row in ideal_rows]
    indices = list(range(len(labels)))
    axes[1].plot(indices, [row["receiver_fidelity"] for row in ideal_rows], marker="o", color="#047857", label="With correction")
    axes[1].plot(indices, [row["receiver_fidelity"] for row in no_correction_rows], marker="s", color="#dc2626", label="No correction")
    axes[1].set_xticks(indices, labels)
    axes[1].set_ylim(0.0, 1.05)
    axes[1].set_ylabel("Receiver fidelity")
    axes[1].set_title("Causality control across message ensemble")
    axes[1].legend()
    axes[1].grid(alpha=0.3)

    figure.suptitle("Project Event Horizon — Phase V ER=EPR-Inspired State Transfer", fontsize=14, fontweight="bold")
    figure.savefig(os.path.join(OUTPUT_DIR, "phase5_wormhole_transfer.png"), dpi=180)
    plt.close(figure)

    print("Average transfer fidelities:")
    for protocol in protocol_order:
        print(f"  {protocol}: {averages[protocol]:.12f}")
    print("Phase V Python integration harness: ALL TESTS GREEN")


if __name__ == "__main__":
    main()
