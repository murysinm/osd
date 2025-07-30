import matplotlib.pyplot as plt
import numpy as np

def read_data(filename):
    with open(filename, 'r') as file:
        blocks = []
        while True:
            line = file.readline().strip()
            if not line:
                break

            block_info = list(map(int, line.split()))

            w_max = int(file.readline().strip())

            points = []
            while True:
                line = file.readline().strip()
                if not line:
                    break
                if line == "---":
                    break
                x, y = map(float, line.split())
                points.append((x, y))

            blocks.append((block_info, w_max, points))

    return blocks

def plot_graph(blocks):
    plt.figure(figsize=(10, 6))

    for block_info, w_max, points in blocks:
        x_vals = [p[0] for p in points]
        y_vals = [p[1] for p in points]

        plt.plot(x_vals, y_vals, label=f"({block_info[0]}, {block_info[1]}), w_max = {w_max}", marker='o')

    plt.yscale('log')
    plt.xlabel('E_b / N_0 (db)')
    plt.ylabel('World/error rate')
    plt.legend()
    plt.grid(True, which="both", ls="--", linewidth=0.5)

    plt.savefig('./graph.png', format='png')

filename = './data.txt'
blocks = read_data(filename)
plot_graph(blocks)
