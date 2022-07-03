# email-db

email-db takes an [.mbox file](https://en.wikipedia.org/wiki/Mbox) (the default format when exporting from gmail and other services), and writes summaries of your emails into an SQLite database so you can do analysis on them with SQLite.

## Build the application

```
make build_all
```

The built application is create at `build/email-db`.

## Run the application to build the sqlite database

```
build/email-db path/to/mbox/file.mbox path/to/new/sqlite.db
```

## To explore

- Export your emails in `.mbox` format (e.g. [takeout.google.com](https://takeout.google.com) for gmail users)
- Install sqlite (ubuntu: `apt install libsqlite3-dev`)
- `sqlite3 path/to/new/sqlite.db`
- explore with SQL!
  - e.g. to see who has been sending lots of emails: `SELECT count(*), from_address FROM emails GROUP BY from_address ORDER BY count(*) asc;`
