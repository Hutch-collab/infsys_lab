import http from 'k6/http';
import { check, sleep } from 'k6';
export const options = { vus: 17, duration: '18s' };
export default function () {
  check(http.get('http://localhost:8080/tasks'), { ok: (r) => r.status === 200 });
  sleep(0.22);
}
