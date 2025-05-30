#!/bin/bash
set -e

BASE_DIR="$HOME/files"
WORDS=("test" "secret" "code" "linux" "unix" "hello" "world" "example" "random" "data")

echo "Удаление $BASE_DIR..."
rm -rf "$BASE_DIR"

echo "Создание $BASE_DIR..."
mkdir -p "$BASE_DIR/dir1/subdir"
mkdir -p "$BASE_DIR/.hidden/.subhidden"

generate_file_with_words() {
  local path="$1"
  local guaranteed_word="test"
  
  {
    echo "$guaranteed_word"

    for _ in {1..30}; do
      word="${WORDS[$RANDOM % ${#WORDS[@]}]}"
      printf "%s " "$word"
      tr -dc 'a-zA-Z ' </dev/urandom | head -c $((RANDOM % 40 + 10))
      echo
    done

    echo "$guaranteed_word"
  } > "$path"
}

echo "Генерация файлов. в них 'test' и случайные слова..."

for i in {1..5}; do
  generate_file_with_words "$BASE_DIR/file$i.txt"
done

for i in {1..3}; do
  generate_file_with_words "$BASE_DIR/.hidden/.file$i.log"
done

for i in {1..3}; do
  generate_file_with_words "$BASE_DIR/dir1/subdir/file$i.txt"
done

echo "Структура с рандомнвми словами создана в $BASE_DIR"