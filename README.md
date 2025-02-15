# reservation_server

## Description

This is a simple reservation server that allows users to make reservations for a exam. The server is written in C++ with workflow by sogou.

## Dependencies

- [workflow](https://github.com/sogou/workflow) A C++ library for high-performance server applications.
- [boost.json](https://boost.org/libs/json) A collection of portable C++ source libraries.
- [libpqxx](https://pqxx.org/development/libpqxx/) A C++ client API for PostgreSQL.

**Note:** We does not depend on ozo which is likely to be better than libpqxx because it is not compatiable with my current system.

## SQL Schema

### User Table

User table is used to store the information of users, the name of which is `user_info`.

The `user_info` table has the following columns:

- `id`: The primary key of the table, which is a big serial.
- `uk_student_id`: The student id of the user, which is unique. This field is allowed to be empty.
- `name`: The name of the user, which is allowed to be empty.
- `username`: The username of the user, which is not allowed to be empty.
- `uk_email`: The email of the user, which is unique.
- `md5_password`: The md5 hash of the password of the user.
- `gmt_create`: The time when the user is created.
- `gmt_modified`: The time when the user is modified.
- `is_delete`: A boolean field to indicate whether the user is deleted.

The SQL schema of the `user_info` table is as follows:

```sql
CREATE TABLE user_info (
    id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    uk_student_id VARCHAR(100) DEFAULT NULL UNIQUE,
    name VARCHAR(100) DEFAULT NULL, -- Allow NULL
    username VARCHAR(100) NOT NULL,
    uk_email VARCHAR(255) NOT NULL UNIQUE,
    md5_password VARCHAR(33) NOT NULL,
    gmt_create TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    gmt_modified TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    is_deleted BOOLEAN NOT NULL DEFAULT FALSE
);
```

### Student Table

Student table is used to store the information of students, the name of which is `student_info`.

The `student_info` table has the following columns:

- `id`: The primary key of the table, which is a big serial.
- `uk_student_id`: The student id of the student, which is unique.
- `name`: The name of the student.
- `gmt_create`: The time when the student is created.
- `gmt_modified`: The time when the student is modified.
- `is_delete`: A boolean field to indicate whether the student is deleted.

```sql
CREATE TABLE student_info (
    id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    uk_student_id BIGINT NOT NULL UNIQUE,
    name VARCHAR(100) NOT NULL,
    gmt_create TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    gmt_modified TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    is_deleted BOOLEAN NOT NULL DEFAULT FALSE
);
```

Besides, in order to improve the performance of the system, we create the following indices:

```sql
CREATE INDEX idx_stu_info_name ON student_info (name, is_deleted);
```

### Exam Table

Exam table is used to store the information of exams, the name of which is `exam_info`.

The `exam_info` table has the following columns:

- `id`: The primary key of the table, which is a big serial.
- `location`: The location of the exam.
- `duration`: The duration of the exam.
- `gmt_begin`: The begin time of the exam.
- `volume`: The volume of the exam.
- `invigilator_login_token`: The login token of the invigilator.
- `gmt_create`: The time when the exam is created.
- `gmt_modified`: The time when the exam is modified.
- `is_deleted`: A boolean field to indicate whether the exam is deleted.
- `is_avaliable`: A boolean field to indicate whether the exam is avaliable.

```sql
CREATE TABLE exam_info (
    id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    location VARCHAR(255) NOT NULL,
    duration INTERVAL NOT NULL,
    gmt_begin BIGINT NOT NULL CHECK (gmt_begin >= 0),
    volume INT NOT NULL CHECK (volume >= 0),
    invigilator_login_token VARCHAR(255) NOT NULL,
    gmt_create TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    gmt_modified TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    is_deleted BOOLEAN NOT NULL DEFAULT FALSE,
    is_avaliable BOOLEAN NOT NULL DEFAULT TRUE,
    UNIQUE (location, gmt_begin)
);
```

Besides, in order to improve the performance of the system, we create the following indices:

```sql
CREATE INDEX idx_exam_info_location ON exam_info (location, is_deleted);
CREATE INDEX idx_exam_info_gmt_begin ON exam_info (gmt_begin, is_deleted);
CREATE INDEX idx_invigilator_login_token ON exam_info (invigilator_login_token, is_deleted);
```

### Subject Table

Subject table is used to store the information of subjects, the name of which is `subject_info`.

The `subject_info` table has the following columns:

- `id`: The primary key of the table, which is a big serial.
- `subject_code`: The code of the subject.
- `subject_name`: The name of the subject.
- `gmt_available`: The time when the subject is available to reserve the exam.
- `gmt_unavailable`: The time when the subject is unavailable to make reservation.
- `gmt_visible`: The time when the exam of the subject is visible.
- `gmt_invisible`: The time when the exam of the subject is going to be invisible.
- `subject_type`: The type of the subject, determing who will be able to make reservation, student or user.
- `term`: The term of the subject.
- `gmt_create`: The time when the subject is created.
- `gmt_modified`: The time when the subject is modified.
- `is_deleted`: A boolean field to indicate whether the subject is deleted.

```sql
CREATE TABLE subject_info (
    id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    subject_code VARCHAR(50) NOT NULL,
    subject_name VARCHAR(255) NOT NULL,
    gmt_available TIMESTAMP NOT NULL,
    gmt_unavailable TIMESTAMP NOT NULL,
    gmt_visible TIMESTAMP NOT NULL,
    gmt_invisible TIMESTAMP NOT NULL,
    subject_type INT NOT NULL CHECK (subject_type >= 0),
    term INT NOT NULL CHECK (term >= 0),
    gmt_create TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    gmt_modified TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    is_deleted BOOLEAN NOT NULL DEFAULT FALSE,
    UNIQUE (subject_code, term)
);
```

Besides, in order to improve the performance of the system, we create the following indices:

```sql
CREATE INDEX idx_term ON subject_info (term, is_deleted);
CREATE INDEX idx_subject_name ON subject_info (subject_name, is_deleted);
CREATE INDEX idx_code_term ON subject_info (subject_code, term, is_deleted);
```

### Subject-student Table

Subject-student table is used to store the relationship between students and subjects, in other words, reservation information of students, the name of which is `subject_student`.

The `subject_student` table has the following columns:

- `id`: The primary key of the table, which is a big serial.
- `subject_id`: The id of the subject.
- `student_id`: The id of the student.
- `exam_id`: The id of the exam, indicating the reservation of the student.
- `is_signed`: A boolean field to indicate whether the student has signed in the exam.

```sql
CREATE TABLE subject_student (
    id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    subject_id BIGINT NOT NULL CHECK (subject_id > 0),
    student_id BIGINT NOT NULL CHECK (student_id > 0),
    exam_id BIGINT DEFAULT NULL CHECK (exam_id > 0),
    is_signed BOOLEAN NOT NULL DEFAULT FALSE,
    UNIQUE (subject_id, user_id)
);
```

Besides, in order to improve the performance of the system, we create the following indices:

```sql
CREATE INDEX idx_student_stbject_id ON subject_student (subject_id);
CREATE INDEX idx_student_id ON subject_student (student_id);
CREATE INDEX idx_student_exam_id ON subject_student (exam_id);
```

### Subject-user Table

Subject-user table is used to store the relationship between users and subjects, in other words, reservation information of users, the name of which is `subject_user`.

The `subject_user` table has the following columns:

- `id`: The primary key of the table, which is a big serial.
- `subject_id`: The id of the subject.
- `user_id`: The id of the user.
- `exam_id`: The id of the exam, indicating which exam the user has reserved for the corresponding subject.
- `is_signed`: A boolean field to indicate whether the user has signed in the exam.

```sql
CREATE TABLE subject_user (
    id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    subject_id BIGINT NOT NULL CHECK (subject_id > 0),
    user_id BIGINT NOT NULL CHECK (user_id > 0),
    exam_id BIGINT DEFAULT NULL CHECK (exam_id > 0),
    is_signed BOOLEAN NOT NULL DEFAULT FALSE,
    UNIQUE (subject_id, user_id)
);
```

Besides, in order to improve the performance of the system, we create the following indices:

```sql
CREATE INDEX idx_user_stbject_id ON subject_student (subject_id);
CREATE INDEX idx_user_id ON subject_user (user_id);
CREATE INDEX idx_user_exam_id ON subject_user (exam_id);
```

### Exam-subject Table

Exam-subject table is used to store the relationship between exams and subjects, the name of which is `exam_subject`.

The `exam_subject` table has the following columns:

- `id`: The primary key of the table, which is a big serial.
- `exam_id`: The id of the exam.
- `subject_id`: The id of the subject.

```sql
CREATE TABLE exam_subject (
    id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    exam_id BIGINT NOT NULL CHECK (exam_id > 0),
    subject_id BIGINT NOT NULL CHECK (subject_id > 0),
    UNIQUE (exam_id, subject_id)
);
```

Besides, in order to improve the performance of the system, we create the following indices:

```sql
CREATE INDEX idx_subject_exam_id ON exam_subject (exam_id);
CREATE INDEX idx_subject_subject_id ON exam_subject (subject_id);
```

## API Docs

This project provides some RESTful APIs for the client to interact with the server, which is easy for the developers to rebuild and maintain the system.

### user_login

The `user_login` api is used to verify the credit of the user to determine whether the user is allowed to get access to the system.

- Method: `POST`
- URL: `/user/login`
- Request Headers:
  - `Content-Type: application/json`
- Request Body:

  ```json5
  {
      "email": "test@nwpu.edu.cn",
      "password": "password"
  }
  ```

- Response:
  - On success:
    - Status Code: `200`
    - Response Headers:
      - `Content-Type: application/json`
      - `Authorization: Bearer <token>`
    - Response Body:

      ```json5
      {
          "email": "test@nnwpu.edu.cn",
          "username": "test",
          "message": "Success."
      }
      ```

  - On failure:
    - Status Code: `401`
    - Response Headers:
      - `Content-Type: application/json`
    - Response Body:

      ```json5
      {
          "message": "Invalid email or password."
      }
      ```

### user_register

The `user_register` api is used to register a new user to the system, after registration, the user can be automatically logged in.

- Method: `POST`
- URL: `/user/register`
- Request Headers:
  - `Content-Type: application/json`
- Request Body:

  ```json5
  {
      "email": "test@nwpu.edu.cn",
      "username": "test",
      "password": "password"
  }
  ```

- Response:
  - On success:
    - Status Code: `201`
    - Response Headers:
      - `Content-Type: application/json`
      - `Authorization: Bearer <token>`
    - Response Body:

        ```json5
        {
            "email": "test@nwpu.edu.cn",
            "username": "test",
            "message": "Success."
        }
        ```

  - On failure:
    - Status Code: `401`
    - Response Headers:
      - `Content-Type: application/json`
    - Response Body:

      ```json5
      {
          "message": "Invalid email or password."
      }
      ```

### user_addstuinfo

The `user_addstuinfo` api is used to add the student information of a user.

- Method: `POST`
- URL: `/user/addstuinfo`
- Request Headers:
  - `Content-Type: application/json`
  - `Authorization: Bearer <token>`
- Request Body:

    ```json5
    {
        "email": "test@nwpu.edu.cn",
        "password": "password",
        "student_id": "2021000000",
        "name": "stu_s_name"
    }
    ```

- Response:
  - On success:
    - Status Code: `201`
    - Response Headers:
      - `Content-Type: application/json`
    - Response Body:

      ```json5
      {
          "student_id": "2021000000",
          "name": "stu_s_name",
          "message": "Success."
      }
      ```

  - Authentication Failure:
    - Status Code: `401`
    - Response Headers:
      - `Content-Type: application/json`
    - Response Body:

      ```json5
      {
          "message": "Invalid email or password."
      }
      ```
  
  - Student ID Conflict:
    - Status Code: `409`
    - Response Headers:
      - `Content-Type: application/json`
    - Response Body:

      ```json5
      {
          "message": "Student ID already exists."
      }
      ```

### user_removestuinfo

The `user_removestuinfo` api is used to remove the student information of a user.

- Method: `POST`
- URL: `/user/removestuinfo`
- Request Headers:
  - `Content-Type: application/json`
  - `Authorization: Bear <token>`
- Request Body:

    ```json5
    {
        "email": "test@nwpu.edu.cn",
        "password": "password",
    }
    ```

- Response:
  - On success:
    - Status Code: `204`
    - Response Headers:
      - `Content-Type: application/json`
    - Response Body:

      ```json5
      {
          "message": "Success."
      }
      ```

  - Authentication Failure:
    - Status Code: `401`
    - Response Headers:
      - `Content-Type: application/json`
    - Response Body:

      ```json5
      {
          "message": "Invalid email or password."
      }
      ```

### user_delete

The `user_delete` api is used to delete the user from the system if the user is no longer needed.

- Method: `DELETE`
- URL: `/user/delete`
- Request Headers:
  - `Authorization: Bearer <token>`
- Response:
  - On success:
    - Status Code: `204`
    - Response Headers:
      - `Content-Type: application/json`
    - Response Body:

      ```json5
      {
          "message": "Success."
      }
      ```

  - Failure:
    - Status Code: `401`
    - Response Headers:
      - `Content-Type: application/json`
    - Response Body:

      ```json5
      {
          "message": "Failed."
      }
      ```

### user_update

The `user_update` api is used to update the user information, such as the username, email, and password.

- Method: `PUT`
- URL: `/user/update`
- Request Headers:
  - `Content-Type: application/json`
  - `Authorization: Bearer <token>`
- Request Body:

    ```json5
    {
        "email": "test@nwpu.edu.cn",
        "password": "password",
        "newEmail": "new_test@nwpu.edu.cn",
        "newPassword": "new_password",
        "newUsername": "new_test"
    }
    ```

- Response:
  - On success:
    - Status Code: `200`
    - Response Headers:
      - `Content-Type: application/json`
    - Response Body:

      ```json5
      {
          "email": "new_test@nwpu.edu.cn",
          "username": "new_test",
          "password": "new_password",
          "message": "Success."
      }
      ```
  
  - Authentication Failure:
    - Status Code: `401`
    - Response Headers:
      - `Content-Type: application/json`
    - Response Body:

      ```json5
      {
          "message": "Invalid email or password."
      }
      ```
  
  - Information Conflict:
    - Status Code: `409`
    - Response Headers:
      - `Content-Type: application/json`
    - Response Body:

      ```json5
      {
          "message": "Email already exists."
      }
      ```

### subject_fetch

The `subject_fetch` api is used to fetch the visible contest information for the user to make reservations.

- Method: `GET`
- URL: `/subject?type=<type>`
- Response:
  - Success:
    - Status Code: `200`
    - Response Headers:
      - `Content-Type: application/json`
      - `Student-Id: <student_id>`
      - `Authorization: Bearer <token>`
    - Response Body:

      ```json5
      {
          "subjects": [
              {
                  "subject_code": "U10G13015",
                  "subject_name": "程序设计基础（C++）",
                  "term": 202401,
                  "gmt_available": 1733328000, 
                  // 2024-12-05 00:00:00
                  "gmt_unavailable": 1733932800, 
                  // 2024-12-12 00:00:00
                  "gmt_visible": 1732982400, 
                  // 2024-12-01 00:00:00
                  "gmt_invisible": 1734624000, 
                  // 2024-12-20 00:00:00
                  "subject_type": 4 // Exam
              }
          ]
      }
      ```

  - Not a Valid Student:
    - Status Code: `401`
    - Response Headers:
      - `Content-Type: application/json`
    - Response Body:

      ```json5
      {
          "message": "Not a valid student."
      }
      ```

**Note**: The `Student-Id` is required only when the type of the request subject is 4 (Exam).

### subject_add

The `subject_add` api is used to make a user select a certain subject.

- Method: `POST`
- URL: `/subject`
- Request Headers:
  - `Content-Type: application/json`
  - `Authorization: Bearer <token>`
- Request Body:

    ```json5
    {
        "subject_code": "U10G13015",
        "term": 202401,
        "email": "test@nwpu.edu.cn",
        "password": "password"
    }
    ```

- Response:
  - Success:
    - Status Code: `201`
    - Response Headers:
      - `Content-Type: application/json`
      - `Authorization: Bearer <token>`
    - Response Body:

      ```json5
      {
          "subject_code": "U10G13015",
          "term": 202401,
          "message": "Success."
      }
      ```
  
  - Authentication Failure:
    - Status Code: `401`
    - Response Headers:
      - `Content-Type: application/json`
    - Response Body:

      ```json5
      {
          "message": "Invalid email or password."
      }
      ```

### exam_list

The `exam_list` api is used to list all the exams of a certain subject which are available for the user to make reservations.

- Method: `GET`
- URL: `/exam?subject_code=<subject_code>&term=<term>`
- Response:
  - Success:
    - Status Code: `200`
    - Response Headers:
      - `Content-Type: application/json`
    - Response Body:

      ```json5
      {
          "exams": [
              {
                  "location": "Room 101",
                  "duration": 7200, // 2 hours
                  "gmt_begin": 1733328000, 
                  // 2024-12-05 00:00:00
                  "volume": 100,
                  "invigilator_login_token": "token",
                  "is_avaliable": true
              }
          ]
      }
      ```

  - Failure:
    - Status Code: `401`
    - Response Headers:
      - `Content-Type: application/json`
    - Response Body:

      ```json5
      {
          "message": "Request failed."
      }
      ```

## Software Architecture

### Router

The router is the component to route the request to the correct handler according to the URL of the request.
With the help of router, the server can easily handle the request and response to the client.

The router contains the following components:

- `Handler Array`: An array of handlers to handle the request when meeting the end path of the URL.
The length of the array is 4, which corresponds to the 4 methods of the HTTP request, `GET`, `POST`, `PUT`, and `DELETE`.

- `Router Map`: A map to store the router of the next level. The key of the map is the path of the next level.
For example, if the URL is `/user/login`, the router will first find the handler of the `user` path, and then find the handler of the `login` path in the user's router map.

### Connection Pool

Connection Pool of database is an important part of the whole server. Considering the performance of creating connections, connection pool is a perfect tool of managing connections.

### Context

Context is one of the most important design of this server. The context is self-contained in this server, which means that the server, the connection pool and other contents are included. With the help of context, the server can dynamically modify some key settings of itself without restarting.
