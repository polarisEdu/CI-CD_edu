#!/bin/bash

# Параметры
USER="clefa"  # имя пользователя на второй машине
HOST="172.20.82.48"  # IP адрес второй машины
PORT="2222"  # порт SSH
BUILD_DIR="build"  # Убедитесь, что здесь нет пробелов
TEMP_DIR="/tmp"  # Временная директория на удалённой машине
DEST_DIR="/usr/local/bin"  # Директория на удалённой машине

# # Отобразим содержимое директорий перед проверкой
# echo "Checking contents of directories:"
# ls -l $BUILD_DIR/src/cat/
# ls -l $BUILD_DIR/src/grep/

# # Убедимся, что файлы артефактов существуют
# if [ ! -f "$BUILD_DIR/src/cat/s21_cat" ] || [ ! -f "$BUILD_DIR/src/grep/s21_grep" ]; then
#   echo "Файлы артефактов не найдены: $BUILD_DIR/src/cat/s21_cat и $BUILD_DIR/src/grep/s21_grep"
#   exit 1
# fi

# Копируем файлы во временную директорию на удалённой машине
scp -o StrictHostKeyChecking=no -P $PORT ../cat/s21_cat $USER@$HOST:$TEMP_DIR
scp -o StrictHostKeyChecking=no -P $PORT ../grep/s21_grep $USER@$HOST:$TEMP_DIR

# Проверяем статус последней команды
if [ $? -ne 0 ]; then
  echo "Ошибка при копировании файлов"
  exit 1
fi

ssh -o StrictHostKeyChecking=no -p $PORT $USER@$HOST "sudo mv $TEMP_DIR/s21_cat $DEST_DIR && sudo mv $TEMP_DIR/s21_grep $DEST_DIR"

# Проверяем статус последней команды
if [ $? -ne 0 ]; then
  echo "Ошибка при перемещении файлов в $DEST_DIR"
  exit 1
fi

echo "Файлы успешно скопированы и перемещены в $DEST_DIR на $HOST"
