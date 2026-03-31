#!/bin/bash

CPU_LIMIT=80
MEM_LIMIT=80
DISK_LIMIT=90
LOG_FILE="server_monitor.log"

now() {
    date "+%Y-%m-%d %H:%M:%S"
}

write_log() {
    echo "[$(now)] $1" >> "$LOG_FILE"
}

# reads from /proc/stat, compares idle vs total time
get_cpu() {
    grep 'cpu ' /proc/stat | awk '{printf "%.0f", ($2+$4)*100/($2+$4+$5)}'
}

get_mem() {
    free -m | awk 'NR==2 {printf "%.0f", $3/$2*100}'
}

get_disk() {
    df / | awk 'NR==2 {print $5}' | tr -d '%'
}

get_procs() {
    ps aux | wc -l
}

show_health() {
    cpu=$(get_cpu)
    mem=$(get_mem)
    disk=$(get_disk)
    procs=$(get_procs)

    echo ""
    echo "Time      : $(now)"
    echo "CPU       : ${cpu}%  (limit: ${CPU_LIMIT}%)"
    echo "Memory    : ${mem}%  (limit: ${MEM_LIMIT}%)"
    echo "Disk      : ${disk}% (limit: ${DISK_LIMIT}%)"
    echo "Processes : $procs"
    echo ""

    all_good=1

    if [ "$cpu" -ge "$CPU_LIMIT" ]; then
        echo "!! CPU is at ${cpu}%, above limit"
        write_log "ALERT - CPU at ${cpu}%"
        all_good=0
    fi

    if [ "$mem" -ge "$MEM_LIMIT" ]; then
        echo "!! Memory is at ${mem}%, above limit"
        write_log "ALERT - Memory at ${mem}%"
        all_good=0
    fi

    if [ "$disk" -ge "$DISK_LIMIT" ]; then
        echo "!! Disk is at ${disk}%, above limit"
        write_log "ALERT - Disk at ${disk}%"
        all_good=0
    fi

    if [ $all_good -eq 1 ]; then
        echo "all metrics look fine"
    fi

    write_log "check done - CPU:${cpu}% MEM:${mem}% DISK:${disk}% PROCS:${procs}"
}

is_valid_percent() {
    if [[ "$1" =~ ^[0-9]+$ ]]; then
        if [ "$1" -ge 1 ] && [ "$1" -le 100 ]; then
            return 0
        fi
    fi
    return 1
}

configure_thresholds() {
    echo ""
    echo "current limits: CPU=${CPU_LIMIT}%  MEM=${MEM_LIMIT}%  DISK=${DISK_LIMIT}%"
    echo ""

    read -p "new CPU limit (enter to keep ${CPU_LIMIT}%): " val
    if [ -n "$val" ]; then
        if is_valid_percent "$val"; then
            CPU_LIMIT=$val
            echo "set to ${CPU_LIMIT}%"
        else
            echo "invalid, keeping ${CPU_LIMIT}%"
        fi
    fi

    read -p "new Memory limit (enter to keep ${MEM_LIMIT}%): " val
    if [ -n "$val" ]; then
        if is_valid_percent "$val"; then
            MEM_LIMIT=$val
            echo "set to ${MEM_LIMIT}%"
        else
            echo "invalid, keeping ${MEM_LIMIT}%"
        fi
    fi

    read -p "new Disk limit (enter to keep ${DISK_LIMIT}%): " val
    if [ -n "$val" ]; then
        if is_valid_percent "$val"; then
            DISK_LIMIT=$val
            echo "set to ${DISK_LIMIT}%"
        else
            echo "invalid, keeping ${DISK_LIMIT}%"
        fi
    fi

    echo "done"
}

view_logs() {
    if [ ! -f "$LOG_FILE" ]; then
        echo "no log file yet"
        return
    fi
    echo ""
    echo "--- $LOG_FILE ---"
    cat "$LOG_FILE"
}

clear_logs() {
    read -p "clear all logs? (y/n): " answer
    if [ "$answer" = "y" ] || [ "$answer" = "Y" ]; then
        > "$LOG_FILE"
        echo "cleared"
        write_log "log cleared"
    else
        echo "cancelled"
    fi
}

start_monitoring() {
    echo "monitoring started, press Ctrl+C to stop"
    write_log "continuous monitoring started"
    while true; do
        show_health
        echo "waiting 60 seconds..."
        sleep 60
    done
}

trap 'echo ""; echo "stopped"; write_log "script interrupted"; exit 0' INT

touch "$LOG_FILE" 2>/dev/null
if [ $? -ne 0 ]; then
    echo "warning: cant write to log"
fi

for cmd in free df ps awk grep; do
    if ! command -v "$cmd" &>/dev/null; then
        echo "error: $cmd not found"
        exit 1
    fi
done

write_log "script started"
echo "Server Health Monitor"

while true; do
    echo ""
    echo "1. show health"
    echo "2. configure limits"
    echo "3. view logs"
    echo "4. clear logs"
    echo "5. start monitoring"
    echo "6. exit"
    read -p "choice: " choice

    if [ "$choice" = "1" ]; then
        show_health
    elif [ "$choice" = "2" ]; then
        configure_thresholds
    elif [ "$choice" = "3" ]; then
        view_logs
    elif [ "$choice" = "4" ]; then
        clear_logs
    elif [ "$choice" = "5" ]; then
        start_monitoring
    elif [ "$choice" = "6" ]; then
        write_log "user exited"
        echo "bye!"
        exit 0
    else
        echo "enter a number 1-6"
    fi
done
