import matplotlib.pyplot as plt
import datetime
import numpy as np

pub_file = "rtiProcessed_joyPub_cleaned.txt"
sub_file = "rtiProcessed_joySub.txt"

with open(pub_file, "r") as f_pub, open(sub_file, "r") as f_sub:
    pub_times = [line.strip() for line in f_pub.readlines()]
    sub_times = [line.strip() for line in f_sub.readlines()]

assert len(pub_times) == len(sub_times), "Publisher 和 Subscriber 数据行数不匹配！"

def parse_time(time_str):
    return datetime.datetime.strptime(time_str, "%H:%M:%S.%f")

pub_times = [parse_time(t) for t in pub_times]
sub_times = [parse_time(t) for t in sub_times]

latencies = [(sub - pub).total_seconds() * 1_000_000 for pub, sub in zip(pub_times, sub_times)]  # 转换为微秒

avg_latency = np.mean(latencies)
print(f"Average Latency: {avg_latency:.3f} µs")

plt.figure(figsize=(10, 5))
plt.plot(range(1, len(latencies) + 1), latencies, marker='o', linestyle='-', markersize=4, label="Latency (µs)")
plt.axhline(avg_latency, color='r', linestyle="--", label=f"Avg Latency: {avg_latency:.3f} µs")
plt.axhline(33333, color='g', linestyle="--", label="30Hz Expected Interval (33,333 µs)")  # 添加 30Hz 参考线
plt.xlabel("Data Index")
plt.ylabel("Latency (µs)")
plt.title("Latency per Data Point (µs Precision)")
plt.legend()
plt.grid()
plt.show()