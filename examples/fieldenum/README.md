Field Number Enumerations
===

In `protobuf3`, fields are by default `optional`.
When a field is not assigned (or, if it is not present in the version of the
`.proto` file that the sender was using), a default value will be assigned to
the field when it is deserialized by the message recipient.
This, however, leads to an ambiguity as to whether the field was assigned a
value which happened to be the default value, or whether it was not set at
all by the sender.

Preceeding the invocation of `protoc` with `QT_PROTOBUF_OPTIONS="FIELDENUM"`
results in additional enums being added to the `.qpb.h` files that are generated.
These can be useful to indicate which fields were actually set when the
message was created.
This could be compared to setting a field to `NULL` in a SQL database.
