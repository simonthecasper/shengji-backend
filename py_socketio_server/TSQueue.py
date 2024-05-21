from threading import Thread, Lock


class TSQueue:
    def __init__(self):
        self.queue = []
        self.q_mutex = Lock()
        pass
    
    
    def enqueue(self, message):
        with self.q_mutex:
            self.queue.append(message)
    
    
    def dequeue(self):
        with self.q_mutex:
            ret = self.q_mutex[0]
            self.q_mutex.pop(0)
            return ret

    def is_empty(self):
        with self.q_mutex:
            return len(self.queue) == 0
        