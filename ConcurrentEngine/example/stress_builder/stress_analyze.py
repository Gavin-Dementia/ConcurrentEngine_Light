import pandas as pd
import matplotlib.pyplot as plt
import sys

# Read command line first argument as CSV
if len(sys.argv) > 1:
    csv_name = sys.argv[1]
else:
    csv_name = "stress_report.csv"  # fallback

# Read CSV
df = pd.read_csv(csv_name)

# USE CSV file name as timestamp
timestamp = csv_name.replace("stress_report_", "").replace(".csv","")

# === break FIFO / PRIORITY ===
fifo = df[df["Scheduler"] == "FIFO"].set_index("Tasks")
prio = df[df["Scheduler"] == "PRIORITY"].set_index("Tasks")

# USE CSV file name as timestamp
timestamp = csv_name.replace("stress_report_", "").replace(".csv","")

# === line chart (total time) ===
plt.figure(figsize=(8,6))
plt.plot(fifo.index, fifo["Total_ms"], "r-o", label="FIFO")
plt.plot(prio.index, prio["Total_ms"], "b-o", label="PRIORITY")
plt.xscale("log")
plt.yscale("log")
plt.xlabel("Tasks (log scale)")
plt.ylabel("Total Time (ms, log scale)")
plt.title("Scheduler Performance Comparison")
plt.legend()
plt.grid(True, linestyle="--", linewidth=0.5)
plt.savefig(f"scheduler_compare_{timestamp}.png", dpi=200)
plt.close()

# === diff percentages ===
diff = ((prio["Total_ms"] - fifo["Total_ms"]) / fifo["Total_ms"]) * 100
plt.figure(figsize=(8,6))
plt.plot(diff.index, diff.values, marker="o", linestyle="-", color="purple")
plt.axhline(0, color="black", linewidth=1, linestyle="--")
plt.xscale("log")
plt.xlabel("Tasks (log scale)")
plt.ylabel("PRIORITY vs FIFO differences (%)")
plt.title("PRIORITY compared to FIFO time differences (%)")
plt.grid(True, linestyle="--", linewidth=0.5)
plt.savefig(f"scheduler_diff_{timestamp}.png", dpi=200)
plt.close()

# === Report Output (Combined Table) ===
compare = pd.DataFrame({
    "FIFO_Total_ms": fifo["Total_ms"],
    "PRIORITY_Total_ms": prio["Total_ms"],
    "Diff_%": diff
})
compare.to_csv(f"scheduler_compare_table_{timestamp}.csv", index=True)

print(f"✅ Output Accomplished (timestamp: {timestamp})")
print(f" - CSV: {csv_name}")
print(f" - scheduler_compare_{timestamp}.png")
print(f" - scheduler_diff_{timestamp}.png")
print(f" - scheduler_compare_table_{timestamp}.csv")
