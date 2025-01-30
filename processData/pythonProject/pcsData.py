
input_file = "steeringWheel_subscriber.txt"
output_file = "rtiProcessed_stwSub.txt"

remove_text = "receive steering wheel data at: 2025-01-30 "

with open(input_file, "r", encoding="utf-8") as infile, open(output_file, "w", encoding="utf-8") as outfile:
    for line in infile:
        processed_line = line.replace(remove_text, "").strip()
        outfile.write(processed_line + "\n")

print(f"processed data is saved in: {output_file}")