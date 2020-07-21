# email-db

email-db writes summaries of your emails into an SQLite database so you can do analysis on them with SQL.

## Build the application

```
make build_all
```

## Run the application to build the sqlite database

```
build/email-db path/to/mbox/file.mbox path/to/new/sqlite.db
```

## To explore

- Install sqlite (ubuntu: `apt install libsqlite3-dev`)
- sqlite3 path/to/new/sqlite.db
- explore with SQL!
  - e.g. to see who has been sending lots of emails: `SELECT count(*), from_address FROM emails GROUP BY from_address ORDER BY count(*) asc;`
