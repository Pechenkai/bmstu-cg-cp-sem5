import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("render_results.csv")

plt.figure(figsize=(10, 6))
plt.plot(df["size"], df["average_time_ms"], marker="o", linestyle="-")
plt.xlabel("Размер, ед.")
plt.ylabel("Среднее время, мс.")
plt.title("Зависимость времени рендеринга от размера высотной карты")
plt.grid()
plt.savefig("render.png")
plt.show()