chmod +x ./toolkit/format.sh
bash ./toolkit/format.sh

git add .
git commit -m "upd: lazily push (date: $(date))"
git push --force