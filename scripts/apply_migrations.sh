#!/bin/bash

set -euo pipefail

DB_URL="postgresql://postgres:qwerty@localhost:5436/postgres"
MIGRATIONS_DIR="../schemas/postgresql/auth/migrations"
MIGRATIONS_LIST="../schemas/postgresql/auth/migrations.txt"

while read -r migration; do
    echo "Applying migration: $MIGRATIONS_DIR/$migration"
    psql "$DB_URL" -f "$MIGRATIONS_DIR/$migration"
done < "$MIGRATIONS_LIST"

echo "All migrations applied."
