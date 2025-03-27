DROP TABLE IF EXISTS tokens;
DROP TABLE IF EXISTS reviews;
DROP TABLE IF EXISTS users;
DROP TABLE IF EXISTS moderation;
DROP TABLE IF EXISTS objs;
DROP TABLE IF EXISTS tmp_objs;

CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    username TEXT NOT NULL UNIQUE,
    password TEXT NOT NULL,
    registration_date TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    role TEXT NOT NULL
);

CREATE TABLE tokens (
    token TEXT PRIMARY KEY NOT NULL,
    user_id INT NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    expiry_date TIMESTAMPTZ NOT NULL
);

CREATE TABLE moderation (
    id SERIAL PRIMARY KEY,
    type TEXT NOT NULL,
    latitude DOUBLE PRECISION NOT NULL,
    longitude DOUBLE PRECISION NOT NULL,
    comment TEXT NOT NULL,
    attachments TEXT NOT NULL
);

CREATE TABLE objs (
    id SERIAL PRIMARY KEY,
    latitude DOUBLE PRECISION NOT NULL,
    longitude DOUBLE PRECISION NOT NULL,
    name TEXT NOT NULL,
    internal_amenities BOOLEAN NOT NULL,
    external_amenities BOOLEAN NOT NULL,
    rating REAL NOT NULL,
    attachments TEXT NOT NULL
);

CREATE TABLE tmp_objs (
    id SERIAL PRIMARY KEY,
    latitude DOUBLE PRECISION NOT NULL,
    longitude DOUBLE PRECISION NOT NULL,
    type TEXT NOT NULL,
    internal_amenities BOOLEAN NOT NULL,
    external_amenities BOOLEAN NOT NULL,
    rating REAL NOT NULL,
    attachments TEXT NOT NULL
);

CREATE TABLE reviews (
    id SERIAL PRIMARY KEY,
    user_id INT NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    obj_id INT NOT NULL REFERENCES objs(id) ON DELETE CASCADE,
    rating INT NOT NULL,
    review TEXT NOT NULL,
    attachments TEXT NOT NULL
);