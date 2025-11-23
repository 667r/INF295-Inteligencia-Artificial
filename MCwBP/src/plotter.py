import matplotlib.pyplot as plt
import pandas as pd
import os

# Lista de las 5 instancias que prometimos en el informe
instancias = ["eil22.txt", "a48.txt", "c50.txt", "f72.txt", "tai75A.txt"]
results_dir = "results/"
output_dir = "graficos/"

if not os.path.exists(output_dir):
    os.makedirs(output_dir)

for inst in instancias:
    csv_path = os.path.join(results_dir, inst + ".csv")
    if os.path.exists(csv_path):
        try:
            df = pd.read_csv(csv_path)
            plt.figure(figsize=(6, 4))
            plt.plot(df['Iteracion'], df['Profit'], label='Mejor Profit', color='#1f77b4')
            plt.title(f'Convergencia: {inst}')
            plt.xlabel('Iteraciones')
            plt.ylabel('Profit')
            plt.grid(True, alpha=0.3)
            plt.tight_layout()
            
            # Guardar como PNG (nombre limpio: a48.png)
            save_name = inst.replace(".txt", ".png")
            plt.savefig(os.path.join(output_dir, save_name), dpi=300)
            print(f"Generado: {save_name}")
            plt.close()
        except Exception as e:
            print(f"Error en {inst}: {e}")
    else:
        print(f"Falta archivo CSV para: {inst}. Ejecuta el solver primero.")