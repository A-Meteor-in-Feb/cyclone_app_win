import datetime

# 文件路径
pub_file = "rtiProcessed_stwPub.txt"
sub_file = "rtiProcessed_stwSub.txt"
lost_file = "rti_lost_stw.txt"
cleaned_pub_file = "rtiProcessed_stwPub_cleaned.txt"

# 读取 Publisher 和 Subscriber 的时间数据
with open(pub_file, "r") as f_pub, open(sub_file, "r") as f_sub:
    pub_times = [line.strip() for line in f_pub.readlines()]
    sub_times = [line.strip() for line in f_sub.readlines()]

# 解析时间为 datetime 对象
def parse_time(time_str):
    return datetime.datetime.strptime(time_str, "%H:%M:%S.%f")

pub_times_dt = [parse_time(t) for t in pub_times]
sub_times_dt = [parse_time(t) for t in sub_times]

# 设定 latency 允许的时间窗口（假设 50ms 内的数据匹配）
latency_threshold = datetime.timedelta(milliseconds=970)

# 记录丢失的数据
lost_times_dt = []
sub_index = 0

for pub_time in pub_times_dt:
    found = False
    # 在 subscriber 数据中找到匹配的数据
    while sub_index < len(sub_times_dt) and sub_times_dt[sub_index] < pub_time:
        sub_index += 1  # 跳过比当前 pub_time 还要早的数据

    # 检查是否有匹配的数据（允许最大 50ms 误差）
    if sub_index < len(sub_times_dt) and (sub_times_dt[sub_index] - pub_time) <= latency_threshold:
        found = True

    # 如果找不到匹配的数据，则认为这个数据丢失
    if not found:
        lost_times_dt.append(pub_time)

# 将丢失的数据转换回字符串格式
lost_times = [t.strftime("%H:%M:%S.%f") for t in lost_times_dt]

# 写入 rti_lost.txt
with open(lost_file, "w") as f_lost:
    f_lost.write("\n".join(lost_times))
print(f"Lost data: {lost_file}")

# 生成新的 Publisher 文件（去除丢失的数据）
cleaned_pub_times = [t.strftime("%H:%M:%S.%f") for t in pub_times_dt if t not in lost_times_dt]

with open(cleaned_pub_file, "w") as f_clean:
    f_clean.write("\n".join(cleaned_pub_times))
print(f"new pub file: {cleaned_pub_file}")