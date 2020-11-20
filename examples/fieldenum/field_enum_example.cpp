#include "field_enum_example.qpb.h"

int main(int argc, char** argv)
{
    qtprotobuf::examples::SparseMessage msg;
    msg.setNumberOfKids(0);
    msg.setFieldsUsed({qtprotobuf::examples::SparseMessage::NumberOfKidsProtoFieldNumber});
    return 0;
}
