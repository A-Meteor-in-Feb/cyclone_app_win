import matplotlib.pyplot as plt
import datetime
import numpy as np

# read data
pub_file = "rtiProcessed_imuPub.txt"
sub_file = "rtiProcessed_imuSub.txt"

with open(pub_file, "r") as f_pub, open(sub_file, "r") as f_sub:
    pub_times = [line.strip() for line in f_pub.readlines()]
    sub_times = [line.strip() for line in f_sub.readlines()]

assert len(pub_times) == len(sub_times), "Publisher and Subscriber have unmatched rows！"

# parse time string
def parse_time(time_str):
    return datetime.datetime.strptime(time_str, "%H:%M:%S.%f")

pub_times = [parse_time(t) for t in pub_times]
sub_times = [parse_time(t) for t in sub_times]

# calculate latency
latencies = [(sub - pub).total_seconds() * 1_000_000 for pub, sub in zip(pub_times, sub_times)]

# calculate average latency
avg_latency = np.mean(latencies)
print(f"Average Latency: {avg_latency:.3f} µs")

# plot latency curve
plt.figure(figsize=(10, 5))
plt.plot(range(1, len(latencies) + 1), latencies, marker='o', linestyle='-', markersize=4, label="Latency (µs)")
plt.axhline(avg_latency, color='r', linestyle="--", label=f"Avg Latency: {avg_latency:.3f} µs")
plt.axhline(50000, color='g', linestyle="--", label="20Hz Expected Interval (50,000 µs)")  # add 50,000µs
plt.xlabel("Data Index")
plt.ylabel("Latency (µs)")
plt.title("Latency per Data Point (µs Precision)")
plt.legend()
plt.grid()
plt.show()