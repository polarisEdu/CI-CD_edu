#!/bin/bash

URL="https://api.telegram.org/bot7621788716:AAHTvV21oJqX6JlTT7CpuK5tYERlbjDHzhU/sendMessage"
TEXT="Stage $1: $2%0A%0AProject:+$3%0AURL:+$4/pipelines/$5/%0ABranch:+$6"

curl -s --max-time 30 -d "chat_id=1984990660&disable_web_page_preview=1&text=$TEXT" $URL > /dev/null