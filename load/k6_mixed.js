import http from 'k6/http';
import { check, sleep } from 'k6';
export const options = { vus: 13, duration: '18s' };
export default function () {
  if (__ITER % 2 === 0) {
    const body = JSON.stringify({ title: `m${__ITER}`, assignee: 'e', priority: 3, dept: 'qa' });
    check(http.post('http://localhost:8080/tasks', body, {
      headers: { 'Content-Type': 'application/json' },
    }), { created: (r) => r.status === 201 });
  } else {
    check(http.get('http://localhost:8080/health'), { ok: (r) => r.status === 200 });
  }
  sleep(0.4);
}
