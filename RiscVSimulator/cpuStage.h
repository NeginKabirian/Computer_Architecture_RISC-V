#ifndef CPUSTAGE_H
#define CPUSTAGE_H

enum class CPUStage {
    Fetch1,
    Fetch2,
    Decode,
    Exec,
    Mem,
    WriteBack,
    HALT
};



#endif // CPUSTAGE_H
