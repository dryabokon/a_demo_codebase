# How to Trigger a Jenkins Build

## Overview
This guide explains how to programmatically trigger a Jenkins build using the REST API with proper CSRF protection.

**Jenkins Instance:** `http://127.0.0.1:8080/job/test-job/`

## Quick Start

```bash
# 1. Get fresh CSRF crumb
curl -c cookies.txt -b cookies.txt -s http://127.0.0.1:8080/crumbIssuer/api/json > crumb.json

# 2. Extract crumb value
CRUMB=$(cat crumb.json | grep -o '"crumb":"[^"]*' | cut -d'"' -f4)

# 3. Trigger the build
curl -X POST -b cookies.txt -c cookies.txt \
  http://127.0.0.1:8080/job/test-job/build \
  -H "Jenkins-Crumb: $CRUMB" \
  -w "\nHTTP Status: %{http_code}\n"
```

Expected response: **HTTP 201** (build queued successfully)

## Step-by-Step Process

### 1. Obtain CSRF Crumb

Jenkins requires CSRF tokens for all POST requests. Get a fresh crumb before triggering builds:

```bash
curl -c cookies.txt -b cookies.txt -s http://127.0.0.1:8080/crumbIssuer/api/json > crumb.json
```

This creates/updates two files:
- `cookies.txt` - Session cookies
- `crumb.json` - CSRF token data

**crumb.json structure:**
```json
{
  "_class": "hudson.security.csrf.DefaultCrumbIssuer",
  "crumb": "47b792d2fdf35620506cfa4761f13e0077e3d14290eb245a88f6259b1d04db51",
  "crumbRequestField": "Jenkins-Crumb"
}
```

### 2. Trigger the Build

Using the crumb from step 1:

```bash
curl -X POST \
  -b cookies.txt \
  -c cookies.txt \
  http://127.0.0.1:8080/job/test-job/build \
  -H "Jenkins-Crumb: <crumb_value>" \
  -w "\nHTTP Status: %{http_code}\n"
```

**Parameters:**
- `-X POST` - HTTP POST method
- `-b cookies.txt` - Read cookies (session authentication)
- `-c cookies.txt` - Save cookies (update session)
- `-H "Jenkins-Crumb: ..."` - CSRF token header
- `-w "\nHTTP Status: %{http_code}\n"` - Display HTTP status code

### 3. Verify Build Was Queued

Check job status to see the new build:

```bash
curl -s -b cookies.txt http://127.0.0.1:8080/job/test-job/api/json
```

Look for:
- `lastBuild.number` - Latest build number (should increment)
- `color` - Build status (`*_anime` means currently building)
- `inQueue` - Whether build is queued (true/false)

**Status Colors:**
- `blue` - Success
- `blue_anime` - Building (will succeed)
- `red` - Failure
- `red_anime` - Building (previous failed)
- `yellow` - Unstable
- `grey` - Never built
- `aborted` - Aborted

### 4. Monitor Build Progress

Get specific build details:

```bash
# Replace 22 with actual build number
curl -s -b cookies.txt http://127.0.0.1:8080/job/test-job/22/api/json
```

Key fields:
- `building` - true if still running
- `result` - SUCCESS, FAILURE, ABORTED (null while building)
- `duration` - Time in milliseconds
- `timestamp` - Start time

Get console output (logs):

```bash
curl -s -b cookies.txt http://127.0.0.1:8080/job/test-job/22/consoleText
```

## Triggering Parameterized Builds

If your job accepts parameters, use the `buildWithParameters` endpoint:

```bash
curl -X POST \
  -b cookies.txt \
  -c cookies.txt \
  "http://127.0.0.1:8080/job/test-job/buildWithParameters?PARAM1=value1&PARAM2=value2" \
  -H "Jenkins-Crumb: $CRUMB"
```

Or with JSON payload:

```bash
curl -X POST \
  -b cookies.txt \
  -c cookies.txt \
  http://127.0.0.1:8080/job/test-job/buildWithParameters \
  -H "Jenkins-Crumb: $CRUMB" \
  -H "Content-Type: application/json" \
  -d '{"parameter": [{"name":"PARAM1", "value":"value1"}]}'
```

## Complete Workflow Example

```bash
#!/bin/bash

JENKINS_URL="http://127.0.0.1:8080"
JOB_NAME="test-job"

echo "Step 1: Getting fresh CSRF crumb..."
curl -c cookies.txt -b cookies.txt -s ${JENKINS_URL}/crumbIssuer/api/json > crumb.json

echo "Step 2: Extracting crumb value..."
CRUMB=$(cat crumb.json | grep -o '"crumb":"[^"]*' | cut -d'"' -f4)
echo "Crumb: $CRUMB"

echo "Step 3: Triggering build..."
HTTP_STATUS=$(curl -X POST \
  -b cookies.txt \
  -c cookies.txt \
  ${JENKINS_URL}/job/${JOB_NAME}/build \
  -H "Jenkins-Crumb: $CRUMB" \
  -w "%{http_code}" \
  -s -o /dev/null)

if [ "$HTTP_STATUS" -eq 201 ]; then
    echo "Build triggered successfully! (HTTP $HTTP_STATUS)"

    echo "Step 4: Getting latest build info..."
    sleep 2  # Wait for build to be queued

    BUILD_INFO=$(curl -s -b cookies.txt ${JENKINS_URL}/job/${JOB_NAME}/api/json)
    BUILD_NUM=$(echo $BUILD_INFO | grep -o '"number":[0-9]*' | head -1 | cut -d':' -f2)

    echo "Latest build: #$BUILD_NUM"
    echo "Monitor at: ${JENKINS_URL}/job/${JOB_NAME}/${BUILD_NUM}/"
else
    echo "Failed to trigger build! (HTTP $HTTP_STATUS)"
fi
```

## Response Codes

| Code | Meaning |
|------|---------|
| 201 | Build successfully queued |
| 403 | Forbidden (invalid/missing crumb or no permissions) |
| 404 | Job not found |
| 409 | Build already in queue (if job doesn't allow concurrent builds) |

## Troubleshooting

### Issue: 403 Forbidden

**Cause:** Invalid or missing CSRF crumb

**Solution:**
1. Get a fresh crumb: `curl -c cookies.txt -b cookies.txt -s http://127.0.0.1:8080/crumbIssuer/api/json > crumb.json`
2. Ensure you're using the crumb from `crumb.json`
3. Include `-b cookies.txt -c cookies.txt` in your request

### Issue: Build not starting

**Cause:** Job may not allow concurrent builds and one is already running

**Solution:** Check if build is queued:
```bash
curl -s -b cookies.txt http://127.0.0.1:8080/job/test-job/api/json | grep -E '"inQueue"|"building"'
```

### Issue: Stale cookies

**Cause:** Session expired

**Solution:** Get fresh crumb (which refreshes cookies):
```bash
curl -c cookies.txt -b cookies.txt -s http://127.0.0.1:8080/crumbIssuer/api/json > crumb.json
```

## Best Practices

1. **Always get a fresh crumb** before triggering builds
2. **Save cookies** (`-c cookies.txt -b cookies.txt`) to maintain session
3. **Check HTTP status code** to verify build was queued (201 = success)
4. **Wait a few seconds** after triggering before checking build status
5. **Handle errors gracefully** - check for 403, 404, 409 responses
6. **Don't spam** - Jenkins rate-limits API requests

## Integration with CI/CD Workflow

### After Code Changes

```bash
# 1. Commit and push changes
git add .
git commit -m "Fix bug in run_experiment"
git push

# 2. Trigger Jenkins build
curl -c cookies.txt -b cookies.txt -s http://127.0.0.1:8080/crumbIssuer/api/json > crumb.json
CRUMB=$(cat crumb.json | grep -o '"crumb":"[^"]*' | cut -d'"' -f4)
curl -X POST -b cookies.txt -c cookies.txt \
  http://127.0.0.1:8080/job/test-job/build \
  -H "Jenkins-Crumb: $CRUMB"

# 3. Monitor build
echo "Build triggered! Monitor at http://127.0.0.1:8080/job/test-job/"
```

## Related Documentation

- **[Jenkins API - Retrieve Results](jenkins-retrieve-results.md)** - How to get build status, logs, and artifacts
- **[GitHub Actions API Guide](github-actions-retrieve-results.md)** - Alternative CI/CD platform

## Summary

To trigger a Jenkins build:
1. Get fresh CSRF crumb: `curl -c cookies.txt -b cookies.txt -s http://127.0.0.1:8080/crumbIssuer/api/json > crumb.json`
2. Trigger build: `curl -X POST -b cookies.txt -c cookies.txt http://127.0.0.1:8080/job/test-job/build -H "Jenkins-Crumb: <crumb>"`
3. Verify with HTTP 201 response
4. Monitor at: http://127.0.0.1:8080/job/test-job/

---

**Last Updated:** 2025-10-27
**Jenkins Version:** Compatible with Jenkins 2.x+
**Job Name:** test-job
**Jenkins URL:** http://127.0.0.1:8080
