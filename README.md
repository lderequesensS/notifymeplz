# notifymeplz

Really simple code for sending me notifications through Telegram.

Is this safe?
I don't know

# Dependencies
- libcurl-devel

## Usage
```
<command> | TELEGRAM_BOT_TOKEN=<token> TELEGRAM_CHAT_ID=<id> notifymeplz [OPTION]
```
Or having the token in your environment

## For future me
Since I have horrible memory I will also add how I was testing this, thank me latter leo:
```
echo "Hello world" | notifymeplz -t
>&2 echo "Hello from stderr" | notifymeplz -t
```

Also for memory:
```
echo "Hello Memory" | valgrind notifymeplz -t
```

And remember <C-d> to send EOF to stdin so you could do  just:
```
./notifymeplz
```
