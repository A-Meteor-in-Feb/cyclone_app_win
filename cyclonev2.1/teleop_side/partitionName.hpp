#ifndef PARTITIONNAME_HPP
#define PARTITIONNAME_HPP

#include <string>

class partitionName {

private:
    std::string partition_name; 

public:

    partitionName() : partition_name("none") {}

    void setPartitionName(const std::string& control_partitionName) {

        partition_name = control_partitionName;

    }

    std::string getPartitionName() const {

        return partition_name;

    }

};



#endif 