import pandas as pd
import matplotlib.pyplot as plt
import os

# -----------------------------
# Optimization levels
# -----------------------------
opt_levels = ["O0", "O1", "O2", "O3", "Ofast"]

# -----------------------------
# Load data
# -----------------------------
data = {}
for opt in opt_levels:
    filename = f"result_{opt}.csv"
    if os.path.exists(filename):
        df = pd.read_csv(filename)
        data[opt] = df
    else:
        print(f"{filename} not found, skipped.")

# -----------------------------
# Figure 1: ASM performance under different optimization levels
# -----------------------------
plt.figure(figsize=(8, 6))
for opt, df in data.items():
    plt.plot(df["Size"], df["ASM_time(ms)"], marker='o', label=f"ASM ({opt})")

plt.title("ASM Performance under Different Optimization Levels")
plt.xlabel("Input Size")
plt.ylabel("Average Time (ms)")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("asm_compare.png", dpi=300)
print("✅ Saved asm_compare.png")

# -----------------------------
# Figure 2: C performance under different optimization levels
# -----------------------------
plt.figure(figsize=(8, 6))
for opt, df in data.items():
    plt.plot(df["Size"], df["C_time(ms)"], marker='o', label=f"C ({opt})")

plt.title("C Performance under Different Optimization Levels")
plt.xlabel("Input Size")
plt.ylabel("Average Time (ms)")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("c_compare.png", dpi=300)
print("✅ Saved c_compare.png")

# -----------------------------
# Figures 3–7: ASM vs C for each optimization level
# -----------------------------
for opt, df in data.items():
    plt.figure(figsize=(8, 6))
    plt.plot(df["Size"], df["ASM_time(ms)"], marker='o', label=f"ASM ({opt})")
    plt.plot(df["Size"], df["C_time(ms)"], marker='s', label=f"C ({opt})")
    plt.title(f"C vs ASM Performance ({opt})")
    plt.xlabel("Input Size")
    plt.ylabel("Average Time (ms)")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    outfile = f"compare_{opt}.png"
    plt.savefig(outfile, dpi=300)
    print(f"✅ Saved {outfile}")

plt.show()
