from fastapi import FastAPI
import uvicorn

app = FastAPI()

@app.get("/egg_detection")
def read_root():
    return {"message": "Hello, World!"}


if __name__ == "__main__":
    uvicorn.run("main:app", host="0.0.0.0", port=2992, reload=True)