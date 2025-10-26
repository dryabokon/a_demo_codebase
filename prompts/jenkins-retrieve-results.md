# How to Derive Results from Last Jenkins Build

## Overview
Jenkins instance running at: `http://127.0.0.1:8080/job/test-job/`

## Step-by-Step Process

### 1. **Handle CSRF Protection (Crumb)**
Jenkins requires CSRF tokens for API calls. Get the crumb first:

```bash
curl -c cookies.txt -b cookies.txt -s http://127.0.0.1:8080/crumbIssuer/api/json > crumb.json
```

This returns:
```json
{
  "_class": "hudson.security.csrf.DefaultCrumbIssuer",
  "crumb": "<token_value>",
  "crumbRequestField": "Jenkins-Crumb"
}
```

### 2. **Get Latest Build Information**
Retrieve job details including latest build number:

```bash
curl -s -b cookies.txt http://127.0.0.1:8080/job/test-job/api/json
```

Key fields to extract:
- `lastBuild.number` - Latest build number
- `lastBuild.url` - URL to the build
- `color` - Build status (`blue` = success, `red` = failure, `*_anime` = in progress)
- `lastCompletedBuild` - Last finished build
- `lastSuccessfulBuild` - Last successful build
- `lastFailedBuild` - Last failed build

### 3. **Get Specific Build Details**
Get detailed info about a specific build (e.g., build #18):

```bash
curl -s -b cookies.txt http://127.0.0.1:8080/job/test-job/18/api/json
```

Returns:
- `result` - Build outcome (SUCCESS, FAILURE, ABORTED, etc.)
- `duration` - Build duration in milliseconds
- `timestamp` - When build started
- `building` - Boolean indicating if still running
- `displayName` - Build display name
- `description` - Build description
- `actions` - Array of build actions and parameters

### 4. **Get Console Output/Logs**
Retrieve the full console log:

```bash
curl -s -b cookies.txt http://127.0.0.1:8080/job/test-job/18/consoleText
```

Or get console output as HTML:
```bash
curl -s -b cookies.txt http://127.0.0.1:8080/job/test-job/18/console
```

### 5. **Get Build Artifacts**
List artifacts produced by the build:

```bash
curl -s -b cookies.txt http://127.0.0.1:8080/job/test-job/18/api/json | grep -A 10 '"artifacts"'
```

Download specific artifact:
```bash
curl -b cookies.txt -o artifact.zip http://127.0.0.1:8080/job/test-job/18/artifact/path/to/artifact.zip
```

### 6. **Trigger a New Build**
To trigger a build programmatically:

```bash
# Get fresh crumb
CRUMB=$(cat crumb.json | grep -o '"crumb":"[^"]*' | cut -d'"' -f4)

# Trigger build
curl -X POST -b cookies.txt -c cookies.txt \
  http://127.0.0.1:8080/job/test-job/build \
  -H "Jenkins-Crumb: $CRUMB" \
  -w "\nHTTP Status: %{http_code}\n"
```

Expected response: HTTP 201 (build queued successfully)

## Key API Endpoints

| Endpoint | Purpose |
|----------|---------|
| `/crumbIssuer/api/json` | Get CSRF token |
| `/job/test-job/api/json` | Get job overview |
| `/job/test-job/lastBuild/api/json` | Get latest build details |
| `/job/test-job/{number}/api/json` | Get specific build details |
| `/job/test-job/{number}/consoleText` | Get console output (plain text) |
| `/job/test-job/{number}/artifact/` | Access build artifacts |
| `/job/test-job/build` | Trigger new build (POST) |
| `/job/test-job/buildWithParameters` | Trigger parameterized build (POST) |

## Important Notes

1. **Cookie Management**: Always use `-c cookies.txt -b cookies.txt` to maintain session
2. **CSRF Protection**: Include `Jenkins-Crumb` header for POST requests
3. **Build Status Colors**:
   - `blue` = Success
   - `red` = Failure
   - `yellow` = Unstable
   - `*_anime` = Currently building
   - `grey` = Pending/Never built
   - `aborted` = Aborted

4. **Response Format**: Add `?pretty=true` for pretty-printed JSON
5. **Tree Parameter**: Use `tree=` parameter to filter JSON response fields

## Example Complete Workflow

```bash
# 1. Get crumb and save cookies
curl -c cookies.txt -b cookies.txt -s http://127.0.0.1:8080/crumbIssuer/api/json > crumb.json

# 2. Get latest build number and status
curl -s -b cookies.txt http://127.0.0.1:8080/job/test-job/api/json | \
  grep -E '"lastBuild"|"color"'

# 3. Get specific build details
BUILD_NUM=18
curl -s -b cookies.txt http://127.0.0.1:8080/job/test-job/${BUILD_NUM}/api/json

# 4. Get console logs
curl -s -b cookies.txt http://127.0.0.1:8080/job/test-job/${BUILD_NUM}/consoleText

# 5. Check for artifacts
curl -s -b cookies.txt http://127.0.0.1:8080/job/test-job/${BUILD_NUM}/api/json | \
  grep -A 5 '"artifacts"'
```

## Current State of test-job
- Latest build: #18 (was building when last checked)
- Previous successful: #17
- Last failed: #16
- Health: 80% (1 out of last 5 builds failed)

---

**Usage**: Save this as a reference and provide it to me along with the Jenkins URL when you need to retrieve build results in the future.
