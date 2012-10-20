set(name StubSys)
set(description "Micro-kernel for running a single program.")
set(supported "malloc" "free" "errno" "exit")
set(files
    start.dasm16
    memory.dasm16
    disk.dasm16
    api.dasm16
)
