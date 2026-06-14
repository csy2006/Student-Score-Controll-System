// 学生成绩管理系统前端交互（调用C++后端API）
const API_BASE = 'http://localhost:8888';

//工具函数
async function fetchJSON(url, options = {}) {
    try {
        const res = await fetch(API_BASE + url, {
            ...options,
            headers: { 'Content-Type': 'application/json', ...(options.headers || {}) }
        });
        return await res.json();
    } catch (e) {
        showToast('无法连接服务器，请确保C++后端已启动', 'error');
        return null;
    }
}

function showToast(msg, type = 'info') {
    const container = document.getElementById('toastContainer');
    const toast = document.createElement('div');
    toast.className = 'toast ' + type;
    toast.textContent = msg;
    container.appendChild(toast);
    setTimeout(() => toast.remove(), 3000);
}

//标签切换
document.querySelectorAll('.nav-tab').forEach(btn => {
    btn.addEventListener('click', () => {
        document.querySelectorAll('.nav-tab').forEach(b => b.classList.remove('active'));
        document.querySelectorAll('.tab-panel').forEach(p => p.classList.remove('active'));
        btn.classList.add('active');
        const panel = document.getElementById('panel-' + btn.dataset.tab);
        if (panel) panel.classList.add('active');

        // 按需加载各面板数据
        switch (btn.dataset.tab) {
            case 'home':      break; // 首页无需加载数据
            case 'students':  loadStudents(); break;
            case 'scores':    loadScoreStudentSelect(); break;
            case 'courses':   loadCourses(); break;
            case 'statistics': loadStatistics(); break;
            case 'ranking':   loadRanking(); break;
            case 'gpa':       loadGpa(); break;
            case 'feedback':  loadFeedbacks(); break;
        }
    });
});


//加载课程列表
let allCourses = [];

async function loadCourses() {
    const grid = document.getElementById('coursesGrid');
    grid.innerHTML = '<div class="loading">正在加载...</div>';
    const data = await fetchJSON('/api/courses');
    if (!data) return;
    allCourses = data.courses;
    renderCourseGrid(allCourses);
    // 更新课程计数
    document.getElementById('courseCount').textContent = allCourses.length;
}

//课程列表
function renderCourseGrid(list) {
    const grid = document.getElementById('coursesGrid');
    if (list.length === 0) {
        grid.innerHTML = '<div class="loading">暂无课程数据，点击"添加课程"创建</div>';
        return;
    }
    grid.innerHTML = list.map(c => `
        <div class="course-card glass-card">
            <div class="course-card-id">课程编号：${c.cId}</div>
            <div class="course-card-name">${c.cName}</div>
            <div class="course-card-actions" style="margin-top:0.6rem; display:flex; gap:0.5rem;">
                <button class="btn btn-sm btn-danger" onclick="deleteCourse(${c.cId}, '${c.cName}')">删除</button>
            </div>
        </div>
    `).join('');
}

//搜索课程
function searchCourse() {
    const kw = document.getElementById('searchCourse').value.trim().toLowerCase();
    if (!kw) {
        renderCourseGrid(allCourses);
        return;
    }
    const filtered = allCourses.filter(c =>
        String(c.cId).includes(kw) || c.cName.toLowerCase().includes(kw)
    );
    renderCourseGrid(filtered);
}

//添加，编辑课程
function showAddCourseModal() {
    document.getElementById('courseModalTitle').textContent = '添加课程';
    document.getElementById('editCourseId').value = '';
    document.getElementById('inputCourseId').value = '';
    document.getElementById('inputCourseId').disabled = false;
    document.getElementById('inputCourseName').value = '';
    document.getElementById('courseModal').classList.add('active');
}

//删除课程
async function deleteCourse(cId, cName) {
    if (!confirm(`确认删除课程【${cName}】（编号：${cId}）吗？此操作不可撤销！`)) return;
    const result = await fetchJSON(`/api/courses/${cId}`, { method: 'DELETE' });
    if (result && result.success) {
        showToast('课程删除成功', 'success');
        loadCourses();
    } else {
        showToast((result && result.msg) || '删除失败', 'error');
    }
}

//保存课程
function closeCourseModal() {
    document.getElementById('courseModal').classList.remove('active');
}

async function saveCourse() {
    const editId = document.getElementById('editCourseId').value;
    const cId   = parseInt(document.getElementById('inputCourseId').value);
    const cName = document.getElementById('inputCourseName').value.trim();
    if (!cId || !cName) { showToast('请填写完整信息', 'error'); return; }

    const result = await fetchJSON('/api/courses', {
        method: 'POST',
        body: JSON.stringify({ cId, cName })
    });

    if (result && result.success) {
        showToast(result.msg || '保存成功', 'success');
        closeCourseModal();
        loadCourses();
    } else {
        showToast((result && result.msg) || '操作失败', 'error');
    }
}


//加载学生列表
let allStudents = [];

async function loadStudents() {
    const tbody = document.getElementById('studentBody');
    tbody.innerHTML = '<tr><td colspan="6" class="loading">正在加载...</td></tr>';
    const data = await fetchJSON('/api/students');
    if (!data) return;
    allStudents = data.students;

    // 填充班级筛选下拉框
    const classSel = document.getElementById('filterClass');
    const classes = [...new Set(allStudents.map(s => s.className).filter(Boolean))].sort();
    classSel.innerHTML = '<option value="">全部班级</option>' +
        classes.map(c => `<option value="${c}">${c}</option>`).join('');

    renderStudentTable(allStudents);
    updateStatus(`已加载 ${allStudents.length} 名学生`);
}

function renderStudentTable(list) {
    const tbody = document.getElementById('studentBody');
    // 控制搜索班级筛选后的提示
    const searchKw = document.getElementById('searchStudent').value.trim();
    const filterCls = document.getElementById('filterClass').value;
    if (list.length === 0 && allStudents.length > 0 && (searchKw || filterCls)) {
        tbody.innerHTML = '<tr><td colspan="6" class="loading">没有匹配的学生</td></tr>';
        return;
    }
    if (list.length === 0) {
        tbody.innerHTML = '<tr><td colspan="6" class="loading">暂无学生数据，点击"重新初始化数据"加载示例</td></tr>';
        return;
    }
    tbody.innerHTML = list.map(s => {
        const avgHtml = s.avg !== undefined && s.avg !== null
            ? `<span class="${s.avg >= 90 ? 'excellent' : s.avg >= 60 ? 'pass' : 'fail'}" style="font-weight:500">${s.avg}</span>`
            : '<span style="color:var(--stone)">-</span>';
        return `
        <tr>
            <td>${s.id}</td>
            <td>${s.name}</td>
            <td>${(s.sex === '男' || s.sex === 'M') ? '♂ 男' : '♀ 女'}</td>
            <td>${s.className}</td>
            <td>${avgHtml}</td>
            <td>
                <div class="action-btns">
                    <button class="btn btn-sm" onclick="editStudent(${s.id})">编辑</button>
                    <button class="btn btn-sm" onclick="gotoStudentScores(${s.id})">成绩</button>
                    <button class="btn btn-sm btn-danger" onclick="deleteStudent(${s.id}, '${s.name}')">删除</button>
                </div>
            </td>
        </tr>
    `}).join('');
}

//搜索学生
document.getElementById('searchStudent').addEventListener('input', applyStudentFilters);

//班级筛选
function filterByClass() {
    document.getElementById('searchStudent').value = '';
    applyStudentFilters();
}

function applyStudentFilters() {
    const kw = (document.getElementById('searchStudent').value.trim() || '').toLowerCase();
    const cls = document.getElementById('filterClass').value;
    let filtered = allStudents;
    if (kw) {
        filtered = filtered.filter(s =>
            s.name.toLowerCase().includes(kw) || String(s.id).includes(kw)
        );
    }
    if (cls) {
        filtered = filtered.filter(s => s.className === cls);
    }
    renderStudentTable(filtered);
}

//添加/编辑学生
function showAddStudentModal() {
    document.getElementById('studentModalTitle').textContent = '添加学生';
    document.getElementById('editStudentId').value = '';
    document.getElementById('inputStudentId').value = '';
    document.getElementById('inputStudentId').disabled = false;
    document.getElementById('inputStudentName').value = '';
    document.getElementById('inputStudentSex').value = 'M';
    document.getElementById('inputStudentClass').value = '';
    document.getElementById('studentModal').classList.add('active');
}

async function editStudent(id) {
    const data = await fetchJSON('/api/students/' + id);
    if (!data || data.error) { showToast('学生不存在', 'error'); return; }
    document.getElementById('studentModalTitle').textContent = '编辑学生';
    document.getElementById('editStudentId').value = id;
    document.getElementById('inputStudentId').value = id;
    document.getElementById('inputStudentId').disabled = true;
    document.getElementById('inputStudentName').value = data.name;

    // 统一后端可能返回的 'M'/'F' 或 '男'/'女'
    const sexVal = (data.sex === 'M' || data.sex === '男') ? 'M' : 'F';
    document.getElementById('inputStudentSex').value = sexVal;
    document.getElementById('inputStudentClass').value = data.className;
    document.getElementById('studentModal').classList.add('active');
}

function closeStudentModal() {
    document.getElementById('studentModal').classList.remove('active');
}

async function saveStudent() {
    //收集表单数据
    const editId = document.getElementById('editStudentId').value;
    const id   = parseInt(document.getElementById('inputStudentId').value);
    const name = document.getElementById('inputStudentName').value.trim();
    const sex  = document.getElementById('inputStudentSex').value;
    const cls  = document.getElementById('inputStudentClass').value.trim();
    const sexText = sex === 'M' ? '男' : '女';

    if (!id || !name || !cls) { showToast('请填写完整信息', 'error'); return; }

    let result;
    if (editId) {
        //发送PUT请求
        result = await fetchJSON('/api/students/' + editId, {   //HttpServer.cpp中定义的接口
            method: 'PUT',
            body: JSON.stringify({ name, sex: sexText, className: cls })
        });
    } else {
        result = await fetchJSON('/api/students', {
            method: 'POST',
            body: JSON.stringify({ id, name, sex: sexText, className: cls })
        });
    }
    //接受响应并处理结果
    if (result && result.success) {
        showToast(result.msg || '保存成功', 'success');
        closeStudentModal();
        loadStudents();
    } else {
        showToast((result && result.msg) || '操作失败', 'error');
    }
}

async function deleteStudent(id, name) {
    if (!confirm(`确认删除学生【${name}】吗？此操作不可撤销！`)) return;
    const result = await fetchJSON('/api/students/' + id, { method: 'DELETE' });
    if (result && result.success) {
        showToast('删除成功', 'success');
        loadStudents();
        // 如果成绩面板正在显示该学生，清空
        const sel = document.getElementById('scoreStudentSelect');
        if (sel.value == id) sel.value = '';
        document.getElementById('scorePanel').innerHTML = '<div class="score-placeholder">请选择学生查看成绩</div>';
    } else {
        showToast((result && result.msg) || '删除失败', 'error');
    }
}

//成绩管理
async function loadScoreStudentSelect() {
    const sel = document.getElementById('scoreStudentSelect');
    sel.innerHTML = '<option value="">-- 选择学生 --</option>';
    const data = await fetchJSON('/api/students');
    if (!data) return;
    data.students.forEach(s => {
        const opt = document.createElement('option');
        opt.value = s.id;
        opt.textContent = `${s.id} - ${s.name}（${s.className}）`;
        sel.appendChild(opt);
    });
}

async function loadStudentScores() {
    const id = document.getElementById('scoreStudentSelect').value;
    const panel = document.getElementById('scorePanel');
    if (!id) {
        panel.innerHTML = '<div class="score-placeholder">请选择学生查看成绩</div>';
        document.getElementById('scoreChartContainer').style.display = 'none';
        document.getElementById('btnVoiceAll').style.display = 'none';
        return;
    }
    // 切换到单个学生时隐藏全局图表和语音按钮
    document.getElementById('scoreChartContainer').style.display = 'none';
    document.getElementById('btnVoiceAll').style.display = 'none';
    panel.innerHTML = '<div class="loading">正在加载成绩...</div>';
    const data = await fetchJSON('/api/scores/' + id);
    if (!data) return;

    // 构建成绩展示
    let html = `<div class="score-header"><h3>${data.name} 的成绩</h3>`;
    if (data.avg !== null) html += `<div class="score-avg">平均分：<span>${data.avg}</span></div>`;
    html += `</div><div class="score-grid">`;

    const courses = data.scores;
    let hasAny = false;
    for (const [cId, info] of Object.entries(courses)) {
        if (!info) continue;
        hasAny = true;
        const scoreClass = info.score >= 90 ? 'excellent' : info.score >= 60 ? 'pass' : 'fail';
        html += `
            <div class="score-card">
                <div class="score-card-name">${info.cName}</div>
                <div class="score-card-value ${scoreClass}">${info.score}</div>
                <div class="score-card-actions">
                    <button class="btn btn-sm" onclick="editScore(${id}, ${cId}, ${info.score})">修改</button>
                    <button class="btn btn-sm btn-danger" onclick="deleteScore(${id}, ${cId}, '${info.cName}')">删除</button>
                </div>
            </div>
        `;
    }
    if (!hasAny) html += '<div class="score-placeholder">暂无成绩记录，请录入成绩</div>';
    html += `</div>
        <div style="margin-top:1rem;text-align:center;display:flex;gap:0.8rem;justify-content:center;flex-wrap:wrap;">
            <button class="btn btn-voice" onclick="speakStudentScores(${data.id})">语音播报</button>
        </div>`;
    panel.innerHTML = html;
}

//显示所有学生成绩
let allScoresData = null;

async function loadAllScores() {
    const panel = document.getElementById('scorePanel');
    panel.innerHTML = '<div class="loading">正在加载所有成绩...</div>';
    const data = await fetchJSON('/api/scores');
    if (!data || !data.scores) {
        panel.innerHTML = '<div class="score-placeholder">暂无成绩数据</div>';
        return;
    }
    allScoresData = data.scores;

    // 获取课程列表用于表头
    const coursesData = await fetchJSON('/api/courses');
    if (!coursesData) return;
    const courses = coursesData.courses;

    // 构建表格
    let html = '<div class="all-scores-table" style="overflow-x:auto;"><table class="data-table">';
    // 表头
    html += '<thead><tr><th>学号</th><th>姓名</th><th>班级</th>';
    courses.forEach(c => { html += `<th>${c.cName}(${c.cId})</th>`; });
    html += '<th>平均分</th></tr></thead><tbody>';

    // 表体
    data.scores.forEach(stu => {
        html += `<tr>`;
        html += `<td>${stu.id}</td><td>${stu.name}</td><td>${stu.className || ''}</td>`;
        courses.forEach(c => {
            const score = stu[c.cId];
            if (score !== null && score !== undefined) {
                const cls = score >= 90 ? 'excellent' : score >= 60 ? 'pass' : 'fail';
                html += `<td class="${cls}">${score}</td>`;
            } else {
                html += `<td>-</td>`;
            }
        });
        html += `<td>${stu.avg !== null ? stu.avg : '-'}</td>`;
        html += `</tr>`;
    });

    html += '</tbody></table></div>';
    panel.innerHTML = html;

    // 显示语音播报按钮并渲染图表
    document.getElementById('btnVoiceAll').style.display = 'inline-block';
    document.getElementById('btnVoiceAll').classList.remove('speaking');
    document.getElementById('btnVoiceAll').textContent = '语音播报';
    renderScoreChart(data.scores);

    updateStatus(`已加载 ${data.scores.length} 名学生的全部成绩`);
}

//搜索成绩（按学号搜索）
async function searchScore() {
    const kw = document.getElementById('searchScore').value.trim();
    if (!kw) {
        // 如果之前加载了所有成绩，恢复显示；否则提示选择学生
        if (allScoresData) {
            loadAllScores();
        }
        return;
    }

    // 如果所有成绩数据还没有加载，先加载
    if (!allScoresData) {
        const data = await fetchJSON('/api/scores');
        if (!data || !data.scores) return;
        allScoresData = data.scores;
    }

    // 仅按学号搜索
    const filtered = allScoresData.filter(stu => {
        return String(stu.id).includes(kw);
    });

    // 确保课程列表已加载
    let courseList = allCourses;
    if (courseList.length === 0) {
        const cd = await fetchJSON('/api/courses');
        if (cd) courseList = cd.courses;
    }

    // 构建表格（复用loadAllScores的逻辑）
    let html = '<div class="all-scores-table" style="overflow-x:auto;"><table class="data-table">';
    html += '<thead><tr><th>学号</th><th>姓名</th><th>班级</th>';
    courseList.forEach(c => { html += `<th>${c.cName}(${c.cId})</th>`; });
    html += '<th>平均分</th></tr></thead><tbody>';

    filtered.forEach(stu => {
        html += `<tr>`;
        html += `<td>${stu.id}</td><td>${stu.name}</td><td>${stu.className || ''}</td>`;
        courseList.forEach(c => {
            const score = stu[c.cId];
            if (score !== null && score !== undefined) {
                const cls = score >= 90 ? 'excellent' : score >= 60 ? 'pass' : 'fail';
                html += `<td class="${cls}">${score}</td>`;
            } else {
                html += `<td>-</td>`;
            }
        });
        html += `<td>${stu.avg !== null ? stu.avg : '-'}</td>`;
        html += `</tr>`;
    });

    html += '</tbody></table></div>';
    if (filtered.length === 0) {
        html = '<div class="score-placeholder">未找到匹配的成绩记录</div>';
    }
    document.getElementById('scorePanel').innerHTML = html;
    // 搜索模式下隐藏图表和语音播报
    document.getElementById('scoreChartContainer').style.display = 'none';
    document.getElementById('btnVoiceAll').style.display = 'none';
    updateStatus(`找到 ${filtered.length} 条匹配记录`);
}

//清空当前学生全部成绩
async function clearCurrentStudentScores() {
    const id = document.getElementById('scoreStudentSelect').value;
    if (!id) { showToast('请先选择学生', 'error'); return; }
    if (!confirm('确认清空该学生的全部成绩？此操作不可撤销！')) return;
    const result = await fetchJSON(`/api/scores/${id}`, { method: 'DELETE' });
    if (result && result.success) {
        showToast('已清空该学生全部成绩', 'success');
        loadStudentScores();
    } else {
        showToast((result && result.msg) || '操作失败', 'error');
    }
}

//从学生列表跳转到成绩面板
async function gotoStudentScores(id) {
    // 1. 切换到成绩管理tab
    const scoresTab = document.querySelector('.nav-tab[data-tab="scores"]');
    if (scoresTab) scoresTab.click();
    // 2. 等待DOM更新后加载下拉框
    await loadScoreStudentSelect();
    // 3. 选中目标学生
    document.getElementById('scoreStudentSelect').value = id;
    // 4. 加载该学生成绩
    loadStudentScores();
}

async function showAddScoreModal() {
    const id = document.getElementById('scoreStudentSelect').value;
    if (!id) { showToast('请先选择学生', 'error'); return; }
    document.getElementById('editScoreStudentId').value = id;
    document.getElementById('editScoreCId').value = '';
    await buildScoreFormBody(id, null);
    document.getElementById('scoreModal').classList.add('active');
}

async function editScore(id, cId, currentScore) {
    document.getElementById('editScoreStudentId').value = id;
    document.getElementById('editScoreCId').value = cId;
    // 构造以课程ID字符串为key的对象，与buildScoreFormBody的取值方式一致
    const scoreObj = {};
    scoreObj[String(cId)] = { score: currentScore };
    await buildScoreFormBody(id, scoreObj);
    document.getElementById('scoreModal').classList.add('active');
}

async function buildScoreFormBody(studentId, existingScores) {
    const data = await fetchJSON('/api/courses');
    if (!data) return;
    const body = document.getElementById('scoreFormBody');
    body.innerHTML = data.courses.map(c => {
        const key = String(c.cId);
        const val = existingScores && existingScores[key] ? existingScores[key].score : '';
        return `
            <div class="form-group">
                <label>${c.cName}（${c.cId}）</label>
                <input type="number" class="form-input score-input" data-cid="${c.cId}" value="${val}" min="0" max="150" placeholder="请输入成绩">
            </div>
        `;
    }).join('');
}

function closeScoreModal() {
    document.getElementById('scoreModal').classList.remove('active');
    // 重置表单状态，避免第二次打开无反应
    document.getElementById('editScoreStudentId').value = '';
    document.getElementById('editScoreCId').value = '';
    document.getElementById('scoreFormBody').innerHTML = '';
}

async function saveScore() {
    const studentId = parseInt(document.getElementById('editScoreStudentId').value);
    if (isNaN(studentId) || studentId <= 0) {
        showToast('学生ID无效，请关闭后重新打开', 'error');
        return;
    }
    const inputs = document.querySelectorAll('.score-input');
    if (inputs.length === 0) {
        showToast('没有可保存的成绩项，请关闭后重新打开', 'error');
        return;
    }
    let saved = 0, failed = 0, errMsg = '';
    for (let i = 0; i < inputs.length; i++) {
        const inp = inputs[i];
        const cId = parseInt(inp.dataset.cid);
        const score = parseInt(inp.value);
        if (isNaN(score) || score < 0 || score > 150) continue;
        // 逐个保存，遇到错误继续保存其他课程
        const result = await fetchJSON('/api/scores', {
            method: 'POST',
            body: JSON.stringify({ id: studentId, cId, score })
        });
        if (result && result.success) {
            saved++;
        } else {
            failed++;
            errMsg += (result && result.msg) ? result.msg + '；' : '未知错误；';
        }
    }
    if (saved > 0) {
        showToast(`成功保存 ${saved} 条成绩` + (failed > 0 ? `，${failed} 条失败` : ''), 'success');
        closeScoreModal();
        loadStudentScores();
    } else {
        showToast('未保存任何成绩：' + (errMsg || '请检查输入（成绩需为0-150的整数）'), 'error');
    }
}

async function deleteScore(studentId, cId, cName) {
    if (!confirm(`确认删除【${cName}】的成绩吗？`)) return;
    const result = await fetchJSON(`/api/scores/${studentId}/${cId}`, { method: 'DELETE' });
    if (result && result.success) {
        showToast('成绩删除成功', 'success');
        loadStudentScores();
    } else {
        showToast((result && result.msg) || '删除失败', 'error');
    }
}

//成绩统计
async function loadStatistics() {
    const panel = document.getElementById('statisticsPanel');
    panel.innerHTML = '<div class="loading">正在加载统计数据...</div>';
    const data = await fetchJSON('/api/statistics');
    if (!data) return;

    const stats = data.statistics;
    if (!stats || stats.length === 0) {
        panel.innerHTML = '<div class="score-placeholder">暂无统计数据，请先录入成绩</div>';
        return;
    }

    let html = '<div class="stat-grid">';
    stats.forEach(s => {
        html += `
        <div class="stat-card">
            <div class="stat-card-title">课程编号 ${s.cId}</div>
            <div class="stat-card-name">${s.cName}</div>
            <div class="stat-row">
                <span class="stat-label">人数</span>
                <span class="stat-value">${s.count}</span>
            </div>
            <div class="stat-row">
                <span class="stat-label">平均分</span>
                <span class="stat-value highlight">${s.avg}</span>
            </div>
            <div class="stat-row">
                <span class="stat-label">最高分</span>
                <span class="stat-value">${s.max}</span>
            </div>
            <div class="stat-row">
                <span class="stat-label">最低分</span>
                <span class="stat-value">${s.min}</span>
            </div>
        </div>`;
    });
    html += '</div>';
    panel.innerHTML = html;
    updateStatus(`已加载 ${stats.length} 门课程统计`);
}

//成绩排名
async function loadRanking() {
    const panel = document.getElementById('rankingPanel');
    panel.innerHTML = '<div class="loading">正在加载排名数据...</div>';
    const data = await fetchJSON('/api/ranking');
    if (!data) return;

    const ranking = data.ranking;
    if (!ranking || ranking.length === 0) {
        panel.innerHTML = '<div class="score-placeholder">暂无排名数据，请先录入成绩</div>';
        return;
    }

    let html = '<div class="ranking-list">';
    ranking.forEach((r, idx) => {
        const rank = idx + 1;
        let rankClass = 'normal';
        if (rank === 1) rankClass = 'gold';
        else if (rank === 2) rankClass = 'silver';
        else if (rank === 3) rankClass = 'bronze';

        html += `
        <div class="rank-item">
            <div class="rank-num ${rankClass}">${rank}</div>
            <div class="rank-info">
                <div class="rank-name">${r.name}</div>
                <div class="rank-detail">学号：${r.id}　班级：${r.className}</div>
            </div>
            <div class="rank-avg">${r.avg !== null ? r.avg : '-'}</div>
        </div>`;
    });
    html += '</div>';
    panel.innerHTML = html;
    updateStatus(`已加载 ${ranking.length} 名学生排名`);
}

//清空所有学生
async function clearAllStudents() {
    if (!confirm('确认清空所有学生信息？此操作将同时删除所有成绩，不可撤销！')) return;
    const result = await fetchJSON('/api/students/all', { method: 'DELETE' });
    if (result && result.success) {
        showToast('已清空所有学生', 'success');
        loadStudents();
        loadScoreStudentSelect();
    } else {
        showToast((result && result.msg) || '操作失败', 'error');
    }
}

//重新初始化数据
async function reInitData() {
    if (!confirm('确认重新初始化示例数据？将清空现有数据并加载6名示例学生，同时初始化所有成绩。')) return;
    const result = await fetchJSON('/api/students/init', { method: 'POST' });
    if (result && result.success) {
        // 再调用成绩初始化接口，为所有学生重新生成随机成绩
        const r2 = await fetchJSON('/api/scores/init', { method: 'POST' });
        const msg2 = (r2 && r2.msg) ? r2.msg : '成绩初始化完成';
        showToast((result.msg || '数据初始化成功') + '，' + msg2, 'success');
        loadStudents();
        loadScoreStudentSelect();
        loadCourses();
    } else {
        showToast((result && result.msg) || '初始化失败', 'error');
    }
}

//清空所有成绩
async function clearAllScores() {
    if (!confirm('⚠️ 确认清空所有学生的全部成绩？此操作不可撤销！')) return;
    const result = await fetchJSON('/api/scores/all', { method: 'DELETE' });
    if (result && result.success) {
        showToast('已清空所有成绩', 'success');
        // 刷新相关面板
        const sel = document.getElementById('scoreStudentSelect');
        if (sel.value) loadStudentScores();
    } else {
        showToast((result && result.msg) || '操作失败', 'error');
    }
}

//批量录入成绩
let batchMode = 'student'; // 'student' 或 'course'

function showBatchAddScoreModal() {
    document.getElementById('batchType').value = '';
    document.getElementById('batchScoreModal').classList.add('active');
    switchBatchMode('student');
}

function switchBatchMode(mode) {
    batchMode = mode;
    document.getElementById('batchBtnStudent').className = mode === 'student' ? 'btn btn-primary' : 'btn';
    document.getElementById('batchBtnCourse').className = mode === 'course' ? 'btn btn-primary' : 'btn';
    buildBatchForm();
}

async function buildBatchForm() {
    const container = document.getElementById('batchScoreForm');
    container.innerHTML = '<div class="loading">正在加载...</div>';

    const students = await fetchJSON('/api/students');
    const courses = await fetchJSON('/api/courses');
    if (!students || !courses) return;

    if (batchMode === 'student') {
        // 按学生录入：选一个学生，给所有课程打分
        let html = '<div class="form-group"><label>选择学生</label>';
        html += '<select id="batchStudentId" class="select-input" style="width:100%" onchange="refreshBatchForm()">';
        html += '<option value="">-- 选择学生 --</option>';
        students.students.forEach(s => {
            html += `<option value="${s.id}">${s.id} - ${s.name}（${s.className}）</option>`;
        });
        html += '</select></div>';
        html += '<div id="batchScoreFields"></div>';
        container.innerHTML = html;
        // 默认不展开课程
        document.getElementById('batchScoreFields').innerHTML = '<div class="score-placeholder">请先选择学生</div>';
    } else {
        // 按课程录入：选一门课程，给所有学生打分
        let html = '<div class="form-group"><label>选择课程</label>';
        html += '<select id="batchCourseId" class="select-input" style="width:100%" onchange="refreshBatchForm()">';
        html += '<option value="">-- 选择课程 --</option>';
        courses.courses.forEach(c => {
            html += `<option value="${c.cId}">${c.cName}（${c.cId}）</option>`;
        });
        html += '</select></div>';
        html += '<div id="batchScoreFields"></div>';
        container.innerHTML = html;
        document.getElementById('batchScoreFields').innerHTML = '<div class="score-placeholder">请先选择课程</div>';
    }
}

async function refreshBatchForm() {
    const container = document.getElementById('batchScoreFields');
    container.innerHTML = '<div class="loading">加载中...</div>';

    const students = await fetchJSON('/api/students');
    const courses = await fetchJSON('/api/courses');
    if (!students || !courses) return;

    if (batchMode === 'student') {
        const sid = parseInt(document.getElementById('batchStudentId').value);
        if (!sid) { container.innerHTML = '<div class="score-placeholder">请选择学生</div>'; return; }
        let html = '';
        courses.courses.forEach(c => {
            html += `
            <div class="batch-form-group">
                <div class="batch-form-label">${c.cName}（${c.cId}）</div>
                <div class="batch-score-row">
                    <input type="number" class="batch-input" data-cid="${c.cId}" min="0" max="150" placeholder="成绩">
                </div>
            </div>`;
        });
        container.innerHTML = html;
    } else {
        const cid = parseInt(document.getElementById('batchCourseId').value);
        if (!cid) { container.innerHTML = '<div class="score-placeholder">请选择课程</div>'; return; }
        let html = '';
        students.students.forEach(s => {
            html += `
            <div class="batch-form-group">
                <div class="batch-form-label">${s.name}（学号：${s.id}，${s.className}）</div>
                <div class="batch-score-row">
                    <input type="number" class="batch-input" data-sid="${s.id}" min="0" max="150" placeholder="成绩">
                </div>
            </div>`;
        });
        container.innerHTML = html;
    }
}

function closeBatchScoreModal() {
    document.getElementById('batchScoreModal').classList.remove('active');
}

async function saveBatchScores() {
    let saved = 0;
    const inputs = document.querySelectorAll('.batch-input');

    if (batchMode === 'student') {
        const sid = parseInt(document.getElementById('batchStudentId').value);
        if (!sid) { showToast('请选择学生', 'error'); return; }
        for (const inp of inputs) {
            const cid = parseInt(inp.dataset.cid);
            const score = parseInt(inp.value);
            if (isNaN(score) || score < 0 || score > 150) continue;
            const r = await fetchJSON('/api/scores', {
                method: 'POST',
                body: JSON.stringify({ id: sid, cId: cid, score })
            });
            if (r && r.success) saved++;
        }
    } else {
        const cid = parseInt(document.getElementById('batchCourseId').value);
        if (!cid) { showToast('请选择课程', 'error'); return; }
        for (const inp of inputs) {
            const sid = parseInt(inp.dataset.sid);
            const score = parseInt(inp.value);
            if (isNaN(score) || score < 0 || score > 150) continue;
            const r = await fetchJSON('/api/scores', {
                method: 'POST',
                body: JSON.stringify({ id: sid, cId: cid, score })
            });
            if (r && r.success) saved++;
        }
    }

    if (saved > 0) {
        showToast(`成功保存 ${saved} 条成绩`, 'success');
        closeBatchScoreModal();
        loadStudents(); // 刷新平均分
    } else {
        showToast('未保存任何成绩，请检查输入', 'error');
    }
}

//状态栏更新
function updateStatus(text) {
    const bar = document.getElementById('statusBar');
    bar.textContent = '● ' + text;
    bar.style.color = '#27ae60';
}

//初始化
document.addEventListener('DOMContentLoaded', () => {
    // 检测后端连接
    fetchJSON('/api/courses').then(data => {
        if (data) {
            updateStatus('已连接服务器，数据加载完成');
            loadStudents();
            loadCourses();
        }
    });

    // 点击模态框外部关闭
    document.querySelectorAll('.modal-overlay').forEach(overlay => {
        overlay.addEventListener('click', (e) => {
            if (e.target === overlay) {
                overlay.classList.remove('active');
            }
        });
    });
});

/*  保留原有动画效果（玻璃卡片倾斜/滚动显现） */
document.addEventListener('DOMContentLoaded', () => {
    // 滚动显现
    const reveals = document.querySelectorAll('.reveal');
    const observer = new IntersectionObserver((entries) => {
        entries.forEach(entry => {
            if (entry.isIntersecting) {
                entry.target.classList.add('visible');
            }
        });
    }, { threshold: 0.1 });
    reveals.forEach(el => observer.observe(el));

    // 卡片3D倾斜效果
    document.querySelectorAll('.glass-card, .place-card').forEach(card => {
        card.addEventListener('mousemove', (e) => {
            const rect = card.getBoundingClientRect();
            const x = e.clientX - rect.left;
            const y = e.clientY - rect.top;
            card.style.setProperty('--mouse-x', x + 'px');
            card.style.setProperty('--mouse-y', y + 'px');
            const cx = (x / rect.width - 0.5) * 2;
            const cy = (y / rect.height - 0.5) * 2;
            card.style.transform = `perspective(800px) rotateY(${cx * 3}deg) rotateX(${-cy * 3}deg)`;
        });
        card.addEventListener('mouseleave', () => {
            card.style.transform = 'perspective(800px) rotateY(0) rotateX(0)';
            card.style.transition = 'transform 0.3s ease';
        });
        card.addEventListener('mouseenter', () => {
            card.style.transition = 'none';
        });
    });

    // 星级评分
    initStarRating();
});

//多实现的功能
//功能1：GPA 绩点排名系统
async function loadGpa() {
    const panel = document.getElementById('gpaPanel');
    panel.innerHTML = '<div class="loading">正在加载GPA数据...</div>';
    const data = await fetchJSON('/api/gpa');
    if (!data || !data.gpa) {
        panel.innerHTML = '<div class="score-placeholder">暂无GPA数据，请先录入成绩</div>';
        return;
    }

    // 按GPA降序排列
    const list = data.gpa.sort((a, b) => b.gpa - a.gpa);

    if (list.length === 0) {
        panel.innerHTML = '<div class="score-placeholder">暂无GPA数据，请先录入成绩</div>';
        return;
    }

    let html = '<div class="ranking-list">';
    list.forEach((s, idx) => {
        const rank = idx + 1;
        let rankClass = 'normal';
        if (rank === 1) rankClass = 'gold';
        else if (rank === 2) rankClass = 'silver';
        else if (rank === 3) rankClass = 'bronze';

        const gpaClass = s.gpa >= 3.5 ? 'gpa-gold' : '';
        html += `
        <div class="gpa-rank-item">
            <div class="gpa-rank-num ${rankClass}">${rank}</div>
            <div class="gpa-rank-info">
                <div class="gpa-rank-name">${s.name}</div>
                <div class="gpa-rank-detail">学号：${s.id}　班级：${s.className}　课程数：${s.courseCount}</div>
            </div>
            <div class="gpa-rank-value ${gpaClass}">${s.gpa.toFixed(2)}</div>
        </div>`;
    });
    html += '</div>';
    panel.innerHTML = html;
    updateStatus(`已加载 ${list.length} 名学生GPA排名`);
}

//功能2：Excel 导入/导出成绩
// 导出成绩为Excel
function exportExcel() {
    if (!allScoresData || allScoresData.length === 0) {
        showToast('请先加载成绩数据', 'warning');
        return;
    }

    // 构建导出数据
    const rows = [];
    // 表头：学号、姓名、班级、各课程成绩、平均分
    const header = ['学号', '姓名', '班级'];
    if (allCourses.length > 0) {
        allCourses.forEach(c => header.push(c.cName + '(' + c.cId + ')'));
    }
    header.push('平均分');
    rows.push(header);

    allScoresData.forEach(stu => {
        const row = [stu.id, stu.name, stu.className || ''];
        allCourses.forEach(c => {
            const score = stu[c.cId];
            row.push(score !== null && score !== undefined ? score : '');
        });
        row.push(stu.avg !== null ? stu.avg : '');
        rows.push(row);
    });

    const ws = XLSX.utils.aoa_to_sheet(rows);
    const wb = XLSX.utils.book_new();
    XLSX.utils.book_append_sheet(wb, ws, '成绩表');
    XLSX.writeFile(wb, '学生成绩导出' + new Date().toISOString().slice(0, 10) + '.xlsx');
    showToast('成绩导出成功', 'success');
}

// 导入Excel成绩
function importExcel() {
    const input = document.createElement('input');
    input.type = 'file';
    input.accept = '.xlsx,.xls,.csv';
    input.onchange = async function (e) {
        const file = e.target.files[0];
        if (!file) return;

        const reader = new FileReader();
        reader.onload = async function (ev) {
            try {
                const data = new Uint8Array(ev.target.result);
                const wb = XLSX.read(data, { type: 'array' });
                const ws = wb.Sheets[wb.SheetNames[0]];
                const rows = XLSX.utils.sheet_to_json(ws, { header: 1 });

                if (rows.length < 2) {
                    showToast('Excel文件为空或格式不正确', 'error');
                    return;
                }

                // 解析表头：学号、姓名、班级、课程名(cId)
                const header = rows[0];
                const courseMap = []; // 列索引 -> cId
                for (let i = 3; i < header.length - 1; i++) {
                    const h = String(header[i]);
                    const match = h.match(/\((\d+)\)$/);
                    if (match) {
                        courseMap.push({ col: i, cId: parseInt(match[1]) });
                    }
                }

                if (courseMap.length === 0) {
                    showToast('未识别到课程列（格式：课程名(编号)）', 'error');
                    return;
                }

                let imported = 0, skipped = 0;
                for (let r = 1; r < rows.length; r++) {
                    const row = rows[r];
                    if (!row || !row[0]) continue;
                    const sid = parseInt(row[0]);
                    if (isNaN(sid) || sid <= 0) { skipped++; continue; }

                    for (const cm of courseMap) {
                        const scoreVal = row[cm.col];
                        if (scoreVal === '' || scoreVal === null || scoreVal === undefined) continue;
                        const score = parseInt(scoreVal);
                        if (isNaN(score) || score < 0 || score > 150) continue;

                        const result = await fetchJSON('/api/scores', {
                            method: 'POST',
                            body: JSON.stringify({ id: sid, cId: cm.cId, score })
                        });
                        if (result && result.success) imported++;
                    }
                }

                showToast(`导入完成：成功 ${imported} 条，跳过 ${skipped} 行`, 'success');
                if (imported > 0) {
                    loadStudents();
                    loadAllScores();
                }
            } catch (err) {
                showToast('Excel解析失败：' + err.message, 'error');
            }
        };
        reader.readAsArrayBuffer(file);
    };
    input.click();
}

//功能3：成绩可视化图表（柱状图）
let scoreChartInstance = null;

function renderScoreChart(scoreData) {
    const container = document.getElementById('scoreChartContainer');
    const canvas = document.getElementById('scoreChart');

    if (!scoreData || scoreData.length === 0) {
        container.style.display = 'none';
        return;
    }

    // 筛选有成绩的学生
    const valid = scoreData.filter(s => s.avg !== null && s.avg !== undefined);
    if (valid.length === 0) {
        container.style.display = 'none';
        return;
    }

    container.style.display = 'block';

    const labels = valid.map(s => s.name + '(' + s.id + ')');
    const avgData = valid.map(s => s.avg);

    // 按平均分着色
    const bgColors = valid.map(s => {
        if (s.avg >= 90) return 'rgba(39,174,96,0.7)';   // 绿色优秀
        if (s.avg >= 80) return 'rgba(43,95,138,0.7)';    // 蓝色良好
        if (s.avg >= 70) return 'rgba(241,196,15,0.7)';   // 黄色中等
        if (s.avg >= 60) return 'rgba(230,126,34,0.7)';   // 橙色及格
        return 'rgba(231,76,60,0.7)';                      // 红色不及格
    });

    if (scoreChartInstance) {
        scoreChartInstance.destroy();
    }

    scoreChartInstance = new Chart(canvas, {
        type: 'bar',
        data: {
            labels: labels,
            datasets: [{
                label: '平均分',
                data: avgData,
                backgroundColor: bgColors,
                borderColor: bgColors.map(c => c.replace('0.7', '1')),
                borderWidth: 1,
                borderRadius: 6
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            plugins: {
                legend: { display: false },
                tooltip: {
                    callbacks: {
                        label: function (ctx) {
                            return '平均分：' + ctx.raw;
                        }
                    }
                }
            },
            scales: {
                y: {
                    beginAtZero: false,
                    min: 0,
                    max: 100,
                    ticks: {
                        stepSize: 10,
                        callback: function (v) { return v + '分'; }
                    },
                    grid: { color: 'rgba(200,185,168,0.2)' }
                },
                x: {
                    ticks: { font: { size: 11 } },
                    grid: { display: false }
                }
            }
        }
    });
}

//功能4：语音成绩播报（Web Speech API）
let currentUtterance = null;

function speakAllScores() {
    if (!allScoresData || allScoresData.length === 0) {
        showToast('请先加载成绩数据', 'warning');
        return;
    }

    if (!('speechSynthesis' in window)) {
        showToast('您的浏览器不支持语音播报', 'error');
        return;
    }

    const btn = document.getElementById('btnVoiceAll');
    if (window.speechSynthesis.speaking) {
        window.speechSynthesis.cancel();
        btn.classList.remove('speaking');
        btn.textContent = '语音播报';
        return;
    }

    btn.classList.add('speaking');
    btn.textContent = '停止播报';

    // 构建播报文本
    const valid = allScoresData.filter(s => s.avg !== null);
    if (valid.length === 0) {
        showToast('没有可播报的成绩数据', 'warning');
        btn.classList.remove('speaking');
        btn.textContent = '语音播报';
        return;
    }

    let text = '以下是所有学生成绩播报。';
    valid.forEach((s, i) => {
        text += `第${i + 1}位，${s.name}，平均分${s.avg}分。`;
    });

    const utterance = new SpeechSynthesisUtterance(text);
    utterance.lang = 'zh-CN';
    utterance.rate = 0.9;
    utterance.onend = () => {
        btn.classList.remove('speaking');
        btn.textContent = '语音播报';
    };
    utterance.onerror = () => {
        btn.classList.remove('speaking');
        btn.textContent = '语音播报';
    };
    currentUtterance = utterance;
    window.speechSynthesis.speak(utterance);
}

// 播报单个学生成绩
async function speakStudentScores(studentId) {
    if (!('speechSynthesis' in window)) {
        showToast('您的浏览器不支持语音播报', 'error');
        return;
    }

    if (window.speechSynthesis.speaking) {
        window.speechSynthesis.cancel();
    }

    const data = await fetchJSON('/api/scores/' + studentId);
    if (!data || data.error) {
        showToast('获取学生信息失败', 'error');
        return;
    }

    let text = `${data.name}的成绩：`;
    let hasScore = false;
    const courses = data.scores;
    for (const [cId, info] of Object.entries(courses)) {
        if (!info) continue;
        hasScore = true;
        text += `${info.cName}${info.score}分，`;
    }
    if (!hasScore) {
        text += '暂无成绩记录。';
    }
    if (data.avg !== null) {
        text += `平均分${data.avg}分。`;
    }

    const utterance = new SpeechSynthesisUtterance(text);
    utterance.lang = 'zh-CN';
    utterance.rate = 0.9;
    window.speechSynthesis.speak(utterance);
    showToast('正在播报：' + data.name + '的成绩', 'info');
}


// 功能5：课程评价与反馈收集
// 初始化星级评分交互
function initStarRating() {
    const stars = document.querySelectorAll('#starRating .star');
    const ratingInput = document.getElementById('feedbackRating');

    stars.forEach(star => {
        star.addEventListener('click', function () {
            const rating = parseInt(this.dataset.rating);
            ratingInput.value = rating;
            stars.forEach(s => {
                const v = parseInt(s.dataset.rating);
                s.textContent = v <= rating ? '★' : '☆';
                s.classList.toggle('active', v <= rating);
                s.classList.toggle('selected', v <= rating);
            });
        });

        star.addEventListener('mouseenter', function () {
            const rating = parseInt(this.dataset.rating);
            stars.forEach(s => {
                const v = parseInt(s.dataset.rating);
                s.textContent = v <= rating ? '★' : '☆';
                s.classList.toggle('active', v <= rating);
            });
        });

        star.addEventListener('mouseleave', function () {
            const currentRating = parseInt(ratingInput.value) || 0;
            stars.forEach(s => {
                const v = parseInt(s.dataset.rating);
                s.textContent = v <= currentRating ? '★' : '☆';
                s.classList.toggle('active', v <= currentRating);
                s.classList.toggle('selected', v <= currentRating);
            });
        });
    });
}

// 显示评价表单
async function showFeedbackForm() {
    // 加载课程列表到下拉框
    const sel = document.getElementById('feedbackCourseId');
    sel.innerHTML = '<option value="">-- 选择课程 --</option>';
    const data = await fetchJSON('/api/courses');
    if (data && data.courses) {
        data.courses.forEach(c => {
            const opt = document.createElement('option');
            opt.value = c.cId;
            opt.textContent = `${c.cName}（${c.cId}）`;
            sel.appendChild(opt);
        });
    }

    // 重置表单
    document.getElementById('feedbackRating').value = '0';
    document.getElementById('feedbackComment').value = '';
    document.querySelectorAll('#starRating .star').forEach(s => {
        s.textContent = '☆';
        s.classList.remove('active', 'selected');
    });

    document.getElementById('feedbackModal').classList.add('active');
}

function closeFeedbackModal() {
    document.getElementById('feedbackModal').classList.remove('active');
}

// 提交评价
async function submitFeedback() {
    const cId = parseInt(document.getElementById('feedbackCourseId').value);
    const rating = parseInt(document.getElementById('feedbackRating').value);
    const comment = document.getElementById('feedbackComment').value.trim();

    if (!cId) { showToast('请选择课程', 'error'); return; }
    if (!rating || rating < 1 || rating > 5) { showToast('请选择评分（1-5星）', 'error'); return; }
    if (!comment) { showToast('请输入评价内容', 'error'); return; }

    const result = await fetchJSON('/api/feedback', {
        method: 'POST',
        body: JSON.stringify({ cId, rating, comment })
    });

    if (result && result.success) {
        showToast('评价提交成功', 'success');
        closeFeedbackModal();
        loadFeedbacks();
    } else {
        showToast((result && result.msg) || '提交失败', 'error');
    }
}

// 加载所有评价
async function loadFeedbacks() {
    const list = document.getElementById('feedbackList');
    list.innerHTML = '<div class="loading">正在加载评价...</div>';

    const data = await fetchJSON('/api/feedback');
    if (!data || !data.feedbacks || data.feedbacks.length === 0) {
        list.innerHTML = '<div class="score-placeholder">暂无课程评价，点击"提交评价"写下你的反馈</div>';
        return;
    }

    // 获取课程名称映射
    const coursesMap = {};
    if (allCourses.length === 0) {
        const cd = await fetchJSON('/api/courses');
        if (cd) cd.courses.forEach(c => { coursesMap[c.cId] = c.cName; });
    } else {
        allCourses.forEach(c => { coursesMap[c.cId] = c.cName; });
    }

    let html = '';
    data.feedbacks.forEach((f, idx) => {
        const starsHtml = '★'.repeat(f.rating) + '☆'.repeat(5 - f.rating);
        const cName = coursesMap[f.cId] || '课程' + f.cId;
        const fbIndex = f.index !== undefined ? f.index : idx;
        html += `
        <div class="feedback-item">
            <div class="feedback-item-header">
                <span class="feedback-course">${cName}</span>
                <span class="feedback-stars-display">${starsHtml}</span>
                <span style="font-size:0.8rem;color:var(--stone);">${f.rating}/5</span>
                <button class="btn btn-sm btn-danger feedback-delete-btn" onclick="deleteFeedback(${fbIndex})" title="删除评价">删除</button>
            </div>
            <div class="feedback-comment">${f.comment}</div>
        </div>`;
    });
    list.innerHTML = html;
    updateStatus(`已加载 ${data.feedbacks.length} 条课程评价`);
}

// 删除单条评价
async function deleteFeedback(index) {
    if (!confirm('确认删除这条评价吗？此操作不可撤销！')) return;
    const result = await fetchJSON(`/api/feedback/${index}`, { method: 'DELETE' });
    if (result && result.success) {
        showToast('评价删除成功', 'success');
        loadFeedbacks();
    } else {
        showToast((result && result.msg) || '删除失败', 'error');
    }
}