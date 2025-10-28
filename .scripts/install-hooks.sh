#!/bin/bash
echo "🔧 Installing Git pre-commit hook..."
ln -sf ../../.scripts/pre-commit .git/hooks/pre-commit
echo "✅ Done. Hook installed."

