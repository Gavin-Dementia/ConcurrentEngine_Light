import pandas as pd
import matplotlib.pyplot as plt
import sys

# 讀命令列第一個參數作 CSV
if len(sys.argv) > 1:
    csv_name = sys.argv[1]
else:
    csv_name = "stress_report.csv"  # fallback

# 讀 CSV
df = pd.read_csv(csv_name)

# 使用 CSV 名稱作 timestamp
timestamp = csv_name.replace("stress_report_", "").replace(".csv","")

# === 拆分 FIFO / PRIORITY ===
fifo = df[df["Scheduler"] == "FIFO"].set_index("Tasks")
prio = df[df["Scheduler"] == "PRIORITY"].set_index("Tasks")

# 使用 CSV 檔名作 timestamp
timestamp = csv_name.replace("stress_report_", "").replace(".csv","")

# === 折線圖 (總耗時) ===
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

# === 差異百分比圖 ===
diff = ((prio["Total_ms"] - fifo["Total_ms"]) / fifo["Total_ms"]) * 100
plt.figure(figsize=(8,6))
plt.plot(diff.index, diff.values, marker="o", linestyle="-", color="purple")
plt.axhline(0, color="black", linewidth=1, linestyle="--")
plt.xscale("log")
plt.xlabel("Tasks (log scale)")
plt.ylabel("PRIORITY vs FIFO 差異 (%)")
plt.title("PRIORITY 相對 FIFO 的耗時差異 (%)")
plt.grid(True, linestyle="--", linewidth=0.5)
plt.savefig(f"scheduler_diff_{timestamp}.png", dpi=200)
plt.close()

# === 報表輸出 (合併表格) ===
compare = pd.DataFrame({
    "FIFO_Total_ms": fifo["Total_ms"],
    "PRIORITY_Total_ms": prio["Total_ms"],
    "Diff_%": diff
})
compare.to_csv(f"scheduler_compare_table_{timestamp}.csv", index=True)

print(f"✅ 輸出完成 (timestamp: {timestamp})")
print(f" - CSV: {csv_name}")
print(f" - scheduler_compare_{timestamp}.png")
print(f" - scheduler_diff_{timestamp}.png")
print(f" - scheduler_compare_table_{timestamp}.csv")
