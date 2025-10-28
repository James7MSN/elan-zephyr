#!/bin/bash
# check_newline_eof.sh v2.5
# 檢查檔案是否缺少 EOF newline 或含有 CRLF，並提供修正選項

SCRIPT_VERSION="v2.5"

show_help() {
    echo "check_newline_eof.sh ${SCRIPT_VERSION}"
    echo
    echo "用法："
    echo "  $0 [選項] <目標路徑 | 檔案> ..."
    echo
    echo "選項："
    echo "  --fix-newline    自動修復缺少 EOF 換行"
    echo "  --fix-crlf       自動將 CRLF 轉為 LF（Unix 格式）"
    echo "  --fix-all        同時修復 newline 與 CRLF 問題"
    echo "  -h, --help       顯示此說明訊息"
    echo
    echo "範例："
    echo "  $0 --fix-newline ."
    echo "  $0 --fix-crlf ./drivers"
    echo "  $0 --fix-all soc/elan/*.h"
    exit 0
}

echo "🔍 check_newline_eof.sh ${SCRIPT_VERSION} - Checking for missing EOF newline and CRLF line endings..."

fix_newline=0
fix_crlf=0
targets=()

for arg in "$@"; do
    case "$arg" in
        --fix-newline)
            fix_newline=1
            ;;
        --fix-crlf)
            fix_crlf=1
            ;;
        --fix-all)
            fix_newline=1
            fix_crlf=1
            ;;
        -h|--help)
            show_help
            ;;
        -*)
            echo "❗ 未知選項: $arg"
            show_help
            ;;
        *)
            targets+=("$arg")
            ;;
    esac
done

# 沒指定就預設掃當前目錄
if [ ${#targets[@]} -eq 0 ]; then
    show_help
fi

missing_newline=0
crlf_found=0
fixed_count=0
fixed_files=()

# 掃描所有文字檔（含無副檔名），避免修改 binary
files=$(find "${targets[@]}" -type f ! -path "*/build/*" ! -path "*/.git/*")

for file in $files; do
    if file "$file" | grep -q "text"; then
        # 檢查 EOF newline
        last_char=$(tail -c1 "$file")
        #if [[ "$last_char" != "" ]]; then
	if [[ "$last_char" != "" && "$last_char" != $'\n' ]]; then
	#if [[ "$last_char" != $'\n' ]]; then
	#if ! awk 'END {exit !/.$/}' "$file"; then
            echo "❌ Missing newline: $file"
            missing_newline=$((missing_newline+1))
            if [[ $fix_newline -eq 1 ]]; then
                echo >> "$file"
                echo "✅ Fixed (newline): $file"
                fixed_count=$((fixed_count+1))
		fixed_files+=("$file")
            fi
        fi

        # 檢查 CRLF
        if grep -q $'\r' "$file"; then
            echo "❌ CRLF line endings: $file"
            crlf_found=$((crlf_found+1))
            if [[ $fix_crlf -eq 1 ]]; then
                sed -i 's/\r$//' "$file"
                echo "✅ Fixed (CRLF): $file"
                fixed_count=$((fixed_count+1))
		fixed_files+=("$file")
            fi
        fi
    fi
done

if [[ $missing_newline -eq 0 && $crlf_found -eq 0 ]]; then
    echo "✅ All checked files are clean."
else
    echo "⚠️  Summary:"
    [[ $missing_newline -gt 0 ]] && echo "   Missing newline: $missing_newline"
    [[ $crlf_found -gt 0 ]]      && echo "   Files with CRLF: $crlf_found"
    if [[ $fix_newline -eq 0 && $fix_crlf -eq 0 ]]; then
        echo "👉 建議使用 '.scripts/check_newline_eof.sh --fix-all <file|dir>' 自動修正。"
    fi
fi

if [[ ${#fixed_files[@]} -gt 0 ]]; then
    echo
    [[ $fixed_count -gt 0 ]]     && echo "🛠️  Files fixed: $fixed_count"
    echo "📌 以下檔案已被修改（修正 newline 或 CRLF）："
    for f in "${fixed_files[@]}"; do
        echo "   - $f"
    done

    # 列出重複出現在staged & unstaged的變更
    am_count=$(git status --porcelain | grep AM | wc -l)
    if (( am_count > 0 )); then
        am_change=$(git status --porcelain | grep AM)
        echo
        echo "⚠️  上述檔案已修正，請執行 'git add <file>' 後再進行 commit。"
        echo "$am_change" | while IFS= read -r file; do
            echo "$file"
        done
    fi
fi

