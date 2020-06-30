DROP DATABASE IF EXISTS login_server;
DROP USER IF EXISTS login_server;

CREATE USER login_server;
CREATE DATABASE login_server WITH OWNER login_server;
GRANT ALL PRIVILEGES ON DATABASE login_server TO login_server;

\connect login_server
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

\connect login_server login_server

CREATE TABLE users(
  id uuid DEFAULT uuid_generate_v4() PRIMARY KEY,
  username VARCHAR NOT NULL UNIQUE,
  password_md5 VARCHAR NOT NULL
);

INSERT INTO users(username, password_md5) VALUES
  (
    'tfeng', '5f4dcc3b5aa765d61d8327deb882cf99'
  ),
  (
    'rfeng', '5f4dcc3b5aa765d61d8327deb882cf99'
  );
