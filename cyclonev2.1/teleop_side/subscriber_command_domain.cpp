#include <string>
#include <iostream>

#include "dds/dds.hpp"
#include "shutdownsignal.hpp"
#include "ControlData.hpp"


void update_tele_state(bool online_state, bool connected_state);
void set_control_publisher_partition(std::string partition_name);
void set_control_subscriber_partition(std::string partition_name);


void subscriber_command_domain(int& tele_id, std::atomic<bool>& command_ato, std::atomic<bool>& control_ato) {

	std::string tele_name = "tele" + std::to_string(tele_id);

	int command_domain = 0;

	dds::domain::DomainParticipant command_participant(command_domain);

	dds::sub::qos::SubscriberQos sub_qos;

	dds::core::StringSeq partition_name{ tele_name };

	sub_qos << dds::core::policy::Partition(partition_name);

	dds::sub::Subscriber command_subscriber(command_participant, sub_qos);

	dds::topic::Topic<ControlData::connection_msg> con_topic(command_participant, "connection_msg");
	dds::topic::Topic<ControlData::disconnection_msg> discon_topic(command_participant, "disconnection_msg");

	dds::core::QosProvider provider("ReliableQos.xml");
	auto reader_qos = provider.datareader_qos("myqos::reliable_reader");

	dds::sub::DataReader<ControlData::connection_msg> con_reader(command_subscriber, con_topic, reader_qos);
	dds::sub::DataReader<ControlData::disconnection_msg> discon_reader(command_subscriber, discon_topic, reader_qos);

	dds::sub::LoanedSamples<ControlData::connection_msg> con_samples;
	dds::sub::LoanedSamples<ControlData::disconnection_msg> discon_samples;

	while (!shutdown_requested) {
		con_samples = con_reader.take();

		if (con_samples.length() > 0) {

			dds::sub::LoanedSamples<ControlData::connection_msg>::const_iterator iter;

			for (iter = con_samples.begin(); iter < con_samples.end(); ++iter) {
				
				const ControlData::connection_msg& data = iter->data();
				const dds::sub::SampleInfo& info = iter->info();

				if (info.valid()) {

					std::string vehicle_id = data.vehicle_id();

					if (vehicle_id == "non-matched") {
						std::cout << "non-matched vehicle for now ..." << std::endl;
						update_tele_state(true, false);
						command_ato = true;
					}
					else {
						std::cout << "match to vehicle: " << vehicle_id << std::endl;
						update_tele_state(true, true);
						command_ato = true;

						std::string control_partition_name = data.tele_id() + data.vehicle_id();
						set_control_publisher_partition(control_partition_name);
						set_control_subscriber_partition(control_partition_name);
						control_ato = true;
					}


				}
			}
		}

		discon_samples = discon_reader.take();

		if (discon_samples.length() > 0) {
			dds::sub::LoanedSamples<ControlData::disconnection_msg>::const_iterator iter;

			for (iter = discon_samples.begin(); iter < discon_samples.end(); ++iter) {

				const ControlData::disconnection_msg& data = iter->data();
				const dds::sub::SampleInfo& info = iter->info();

				if (info.valid()) {

					std::cout << "Received Disconnection Msg: " << data.msg() << std::endl;

					update_tele_state(false, false);
					command_ato = true;

					if (control_ato.load()) {
						std::string control_partition_name = "none";
						set_control_publisher_partition(control_partition_name);
						set_control_subscriber_partition(control_partition_name);
						control_ato = false;
					}
					

				}
			}
		}
	}
}