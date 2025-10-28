# 📂 .scripts/ 說明

本目錄包含本專案開發流程所需的輔助工具，例如 pre-commit hook 檢查程式碼格式、換行規則等。請依照以下步驟設定你的開發環境，以避免提交不符合規範的程式碼。

---

## 📌 目前工具清單

| 檔案 | 說明 |
|------|------|
| `pre-commit` | Git pre-commit hook，檢查 staged 檔案是否缺少 newline (`\n`) 或含有 CRLF 換行 |
| `check_newline_eof.sh` | 手動執行 newline 檢查工具，可對任意目錄或檔案掃描並自動修復 |
| `install-hooks.sh` | 安裝 pre-commit hook 的快速指令 |

---

## 🛠️ 安裝 pre-commit hook

此指令會將 .scripts/pre-commit 建立符號連結為 .git/hooks/pre-commit，使每次 git commit 自動執行檢查。
請執行下列指令來啟用 Git pre-commit 檢查：

```bash
./.scripts/install-hooks.sh

(.venv) johnny@PC-05010:~/zephyrproject/elan-zephyr$ ll .git/hooks/pre-commit*
lrwxrwxrwx 1 johnny johnny   25 Jul  2 16:37 .git/hooks/pre-commit -> ../../.scripts/pre-commit*
```

---

## ✅ 檢查項目說明
1. 結尾換行檢查（newline at EOF）
檔案必須以換行字元 (\n) 作結尾。

若缺少換行，會被阻止 commit，請修正或使用工具自動加上。

2. 換行格式檢查（CRLF / LF）
本專案所有文字檔案應使用 UNIX 格式換行（LF）。

若檢測到 Windows 格式換行（CRLF），會被阻止 commit。

可使用 dos2unix 工具修正，或設定編輯器預設使用 LF。

---

### 手動掃描檔案

若你想掃描整個目錄是否有違規的換行格式，可使用下列指令
```bash
.scripts/check_newline_eof.sh <檔案或目錄>
```

### 自動修正檔案

例如自動檢查並修正當前目錄
```bash
.scripts/check_newline_eof.sh --fix-newline <檔案或目錄>
```

例如自動檢查並修正換行格式
```bash
.scripts/check_newline_eof.sh --fix-crlf <檔案或目錄>
```

例如同時自動檢查並修正結尾換行與換行格式
```bash
.scripts/check_newline_eof.sh --fix-all <檔案或目錄>
```

