# How to Derive Results from GitHub Actions

## Overview
Repository: `https://github.com/dryabokon/a_demo_codebase`

## Step-by-Step Process

### 1. **Get Repository Information**
Extract repository owner and name from git remote:

```bash
git remote -v
```

Output format: `https://github.com/{owner}/{repo}.git`

### 2. **List Recent Workflow Runs**
Get the latest workflow runs using GitHub API:

```bash
curl -s https://api.github.com/repos/{owner}/{repo}/actions/runs
```

Or using `gh` CLI (if available):
```bash
gh run list --limit 5
```

Key fields to extract:
- `workflow_runs[].id` - Unique run ID
- `workflow_runs[].name` - Workflow name
- `workflow_runs[].run_number` - Sequential run number
- `workflow_runs[].status` - Run status (queued, in_progress, completed)
- `workflow_runs[].conclusion` - Final result (success, failure, cancelled, skipped)
- `workflow_runs[].created_at` - When run started
- `workflow_runs[].updated_at` - Last update time
- `workflow_runs[].html_url` - Link to run in GitHub UI
- `workflow_runs[].head_commit.message` - Commit message that triggered run

### 3. **Get Specific Run Details**
Get detailed information about a specific workflow run:

```bash
curl -s https://api.github.com/repos/{owner}/{repo}/actions/runs/{run_id}
```

Returns:
- `name` - Workflow name
- `status` - Current status
- `conclusion` - Final outcome
- `run_started_at` - Start timestamp
- `created_at`, `updated_at` - Creation and update times
- `head_branch` - Branch name
- `head_sha` - Commit hash
- `event` - Trigger event (push, pull_request, etc.)
- `actor` - User who triggered the run
- `run_attempt` - Attempt number (for reruns)
- `jobs_url` - URL to get jobs information
- `logs_url` - URL to download logs
- `artifacts_url` - URL to list artifacts

### 4. **Get Job Details**
List all jobs within a workflow run:

```bash
curl -s https://api.github.com/repos/{owner}/{repo}/actions/runs/{run_id}/jobs
```

Key information:
- `jobs[].name` - Job name
- `jobs[].status` - Job status
- `jobs[].conclusion` - Job outcome
- `jobs[].started_at`, `jobs[].completed_at` - Timing info
- `jobs[].steps[]` - Array of steps executed
- `jobs[].steps[].name` - Step name
- `jobs[].steps[].status` - Step status
- `jobs[].steps[].conclusion` - Step outcome
- `jobs[].steps[].number` - Step sequence number
- `jobs[].runner_name` - Runner that executed the job

### 5. **List Artifacts**
Get artifacts produced by the workflow run:

```bash
curl -s https://api.github.com/repos/{owner}/{repo}/actions/runs/{run_id}/artifacts
```

Returns:
- `artifacts[].name` - Artifact name
- `artifacts[].size_in_bytes` - File size
- `artifacts[].archive_download_url` - Download URL (requires auth)
- `artifacts[].expired` - Whether artifact has expired
- `artifacts[].created_at`, `artifacts[].expires_at` - Lifecycle timestamps

### 6. **Download Artifacts**
Download artifact (requires authentication):

```bash
# Using GitHub token
curl -L -H "Authorization: token ${GITHUB_TOKEN}" \
  -o artifact.zip \
  https://api.github.com/repos/{owner}/{repo}/actions/artifacts/{artifact_id}/zip
```

Or using `gh` CLI:
```bash
gh run download {run_id} -n {artifact_name}
```

### 7. **Get Workflow Logs**
Download logs for entire workflow run:

```bash
curl -L -H "Authorization: token ${GITHUB_TOKEN}" \
  https://api.github.com/repos/{owner}/{repo}/actions/runs/{run_id}/logs \
  -o logs.zip
```

Or using `gh` CLI:
```bash
gh run view {run_id} --log
```

### 8. **Trigger New Workflow Run**
Trigger a workflow using repository dispatch or workflow_dispatch:

```bash
# Using workflow_dispatch event
curl -X POST \
  -H "Authorization: token ${GITHUB_TOKEN}" \
  -H "Accept: application/vnd.github.v3+json" \
  https://api.github.com/repos/{owner}/{repo}/actions/workflows/{workflow_id}/dispatches \
  -d '{"ref":"main","inputs":{"param1":"value1"}}'
```

Or using `gh` CLI:
```bash
gh workflow run {workflow_name} --ref main
```

## Key API Endpoints

| Endpoint | Purpose |
|----------|---------|
| `/repos/{owner}/{repo}/actions/runs` | List all workflow runs |
| `/repos/{owner}/{repo}/actions/runs/{run_id}` | Get specific run details |
| `/repos/{owner}/{repo}/actions/runs/{run_id}/jobs` | Get jobs for a run |
| `/repos/{owner}/{repo}/actions/runs/{run_id}/artifacts` | List artifacts |
| `/repos/{owner}/{repo}/actions/artifacts/{artifact_id}/zip` | Download artifact |
| `/repos/{owner}/{repo}/actions/runs/{run_id}/logs` | Download logs |
| `/repos/{owner}/{repo}/actions/runs/{run_id}/rerun` | Rerun workflow |
| `/repos/{owner}/{repo}/actions/workflows` | List all workflows |
| `/repos/{owner}/{repo}/actions/workflows/{workflow_id}/dispatches` | Trigger workflow |

## Important Notes

1. **Authentication**:
   - Public repo data is accessible without auth
   - Artifacts and logs require authentication
   - Use personal access token or `gh` CLI for authenticated requests

2. **Rate Limiting**:
   - Unauthenticated: 60 requests/hour
   - Authenticated: 5000 requests/hour

3. **Status vs Conclusion**:
   - `status`: Current state (queued, in_progress, completed)
   - `conclusion`: Final result (success, failure, cancelled, skipped, null if in progress)

4. **Run Status Values**:
   - `queued` - Waiting to start
   - `in_progress` - Currently running
   - `completed` - Finished (check conclusion for result)

5. **Conclusion Values**:
   - `success` - All jobs succeeded
   - `failure` - At least one job failed
   - `cancelled` - Workflow was cancelled
   - `skipped` - Workflow was skipped
   - `null` - Still in progress

6. **Using WebFetch**:
   - Can fetch public GitHub API endpoints without authentication
   - Use the `prompt` parameter to extract specific information
   - Example: Extract workflow name, status, and conclusion from runs

## Example Complete Workflow

### Using GitHub API (curl)

```bash
# 1. Get latest runs
curl -s https://api.github.com/repos/dryabokon/a_demo_codebase/actions/runs | \
  jq '.workflow_runs[] | {run_number, status, conclusion, created_at}'

# 2. Get specific run details
RUN_ID=18817882088
curl -s https://api.github.com/repos/dryabokon/a_demo_codebase/actions/runs/${RUN_ID} | \
  jq '{name, status, conclusion, run_started_at, head_sha}'

# 3. Get jobs for the run
curl -s https://api.github.com/repos/dryabokon/a_demo_codebase/actions/runs/${RUN_ID}/jobs | \
  jq '.jobs[] | {name, status, conclusion, steps: [.steps[] | {name, conclusion}]}'

# 4. List artifacts
curl -s https://api.github.com/repos/dryabokon/a_demo_codebase/actions/runs/${RUN_ID}/artifacts | \
  jq '.artifacts[] | {name, size_in_bytes, expired}'

# 5. Download artifact (requires auth)
ARTIFACT_ID=4374455353
curl -L -H "Authorization: token ${GITHUB_TOKEN}" \
  -o results.zip \
  https://api.github.com/repos/dryabokon/a_demo_codebase/actions/artifacts/${ARTIFACT_ID}/zip
```

### Using gh CLI

```bash
# 1. List recent runs
gh run list --limit 5

# 2. View specific run
gh run view 18817882088

# 3. View run with logs
gh run view 18817882088 --log

# 4. Download artifacts
gh run download 18817882088

# 5. Trigger new run
gh workflow run "Run Experiment"

# 6. Watch a running workflow
gh run watch 18817882088
```

### Using WebFetch (in Claude Code)

```bash
# Fetch and extract run information
WebFetch(
  url="https://api.github.com/repos/dryabokon/a_demo_codebase/actions/runs",
  prompt="Extract the most recent workflow runs including: workflow name, run number, status, conclusion, created date, and run URL. Show the latest 5 runs."
)

# Get specific run details
WebFetch(
  url="https://api.github.com/repos/dryabokon/a_demo_codebase/actions/runs/18817882088",
  prompt="Extract detailed information about this workflow run including: workflow name, status, conclusion, duration, jobs information, and any artifacts or logs available"
)

# Get job details
WebFetch(
  url="https://api.github.com/repos/dryabokon/a_demo_codebase/actions/runs/18817882088/jobs",
  prompt="Extract information about all jobs in this workflow run including: job names, status, conclusion, duration, and steps executed with their outcomes"
)

# List artifacts
WebFetch(
  url="https://api.github.com/repos/dryabokon/a_demo_codebase/actions/runs/18817882088/artifacts",
  prompt="List all artifacts produced by this workflow run including: artifact names, size, and download URLs"
)
```

## Example Workflow File Analysis

To understand what a workflow does, read the workflow file:

```bash
# Find workflow files
ls .github/workflows/

# Read workflow configuration
cat .github/workflows/run-experiment.yml
```

Common workflow structure:
- `name` - Workflow name
- `on` - Trigger events (push, pull_request, workflow_dispatch, etc.)
- `jobs` - Jobs to execute
- `jobs.{job_id}.runs-on` - Runner OS
- `jobs.{job_id}.steps` - Steps to execute
- `jobs.{job_id}.steps[].uses` - GitHub Action to use
- `jobs.{job_id}.steps[].run` - Shell commands to execute

## Current Repository Example

**Repository**: `dryabokon/a_demo_codebase`

**Workflow**: Run Experiment (`.github/workflows/run-experiment.yml`)
- **Trigger**: Push to main branch
- **Runner**: Windows-latest
- **Python Version**: 3.13
- **Steps**:
  1. Checkout code
  2. Setup Python
  3. Install uv package manager
  4. Sync dependencies and run `python src/main.py`
  5. Save run metadata (JSON with run info, timestamp, commit)
  6. Upload artifacts (output folder + metadata)

**Latest Run (#13)**:
- Status: Completed
- Conclusion: Success
- Duration: 41 seconds
- Commit: c687cc9 - "Add timing functionality to display total program duration"
- Artifact: `results-18817882088` (306 bytes)

---

**Usage**: Use this guide to retrieve and analyze GitHub Actions workflow results. Can be used with curl, gh CLI, or WebFetch tool.
